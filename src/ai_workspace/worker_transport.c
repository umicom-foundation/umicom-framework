/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ai_workspace/worker_transport.c
 *
 * PURPOSE:
 *   Run bounded worker requests without arbitrary hosts, filesystem paths or commands.
 *
 * EDUCATIONAL OVERVIEW:
 *   ComfyUI owns model execution and its queue. This adapter validates the
 *   transport and workflow boundary; it never presents simulated media as output.
 *
 * AUTHOR AND ORGANISATION:
 *   Sammy Hegab
 *   Umicom Foundation
 *
 * LICENCE:
 *   MIT
 *---------------------------------------------------------------------------*/

#include "umicom/ai_workspace/worker_transport.h"
#include <curl/curl.h>
#include <json-c/json.h>
#include <ctype.h>
#include <limits.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* A response is accumulated only for small JSON; media bytes go to the caller. */
typedef struct WorkerReply {
    char *data;
    size_t length;
    size_t limit;
    bool overflow;
    bool sinkFailed;
    UmiAiWorkerWrite sink;
    void *context;
} WorkerReply;

/* Return one fixed diagnostic without exposing prompts, URLs or backend errors. */
static UmiStatus WorkerError(UmiStatus status, char *error, size_t capacity, const char *text)
{
    if (error != NULL && capacity != 0U) (void)snprintf(error, capacity, "%s", text);
    return status;
}

/* Reject invalid UTF-8 before feeding json-c, including overlong and surrogate forms. */
static bool WorkerUtf8(const unsigned char *s, size_t n)
{
    size_t i = 0U;
    while (i < n) {
        unsigned first = s[i++];
        if (first < 0x80U) { if (first == 0U) return false; continue; }
        unsigned count, code, minimum;
        if (first >= 0xC2U && first <= 0xDFU) { count = 1U; code = first & 31U; minimum = 0x80U; }
        else if (first >= 0xE0U && first <= 0xEFU) { count = 2U; code = first & 15U; minimum = 0x800U; }
        else if (first >= 0xF0U && first <= 0xF4U) { count = 3U; code = first & 7U; minimum = 0x10000U; }
        else return false;
        if ((size_t)count > n - i) return false;
        for (unsigned k = 0U; k < count; ++k) {
            unsigned byte = s[i++];
            if ((byte & 0xC0U) != 0x80U) return false;
            code = (code << 6U) | (byte & 63U);
        }
        if (code < minimum || code > 0x10FFFFU || (code >= 0xD800U && code <= 0xDFFFU)) return false;
    }
    return true;
}

/* Represent validated Unicode as equivalent JSON escapes for json-c portability.
 * STRICT parsing in json-c 0.18 can reject non-ASCII input. This mirrors the
 * existing Framework codec strategy without weakening syntax validation. */
static char *WorkerAsciiJson(const char *text, size_t length, size_t *written)
{
    if (length > (SIZE_MAX - 1U) / 6U) return NULL;
    char *out = malloc(length * 6U + 1U);
    if (out == NULL) return NULL;
    const unsigned char *input = (const unsigned char *)text;
    static const char hex[] = "0123456789abcdef";
    size_t count = 0U;
    for (size_t i = 0U; i < length;) {
        unsigned first = input[i++];
        if (first < 0x80U) { out[count++] = (char)first; continue; }
        unsigned extra = first < 0xE0U ? 1U : first < 0xF0U ? 2U : 3U;
        uint32_t code = first & (extra == 1U ? 31U : extra == 2U ? 15U : 7U);
        for (unsigned j = 0U; j < extra; ++j) code = (code << 6U) | (input[i++] & 63U);
        uint32_t units[2] = {code, 0U};
        unsigned n = 1U;
        if (code > 0xFFFFU) { code -= 0x10000U; units[0] = 0xD800U + (code >> 10U); units[1] = 0xDC00U + (code & 0x3FFU); n = 2U; }
        for (unsigned u = 0U; u < n; ++u) {
            out[count++] = '\\'; out[count++] = 'u';
            for (unsigned shift = 16U; shift > 0U; shift -= 4U) out[count++] = hex[(units[u] >> (shift - 4U)) & 15U];
        }
    }
    out[count] = '\0'; *written = count; return out;
}

/* Validate encoding, then keep strict JSON syntax and exact end-of-input checks. */
UmiStatus UmiAiWorkerParseJson(const char *text, size_t length, json_object **out)
{
    if (out == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out = NULL;
    if (text == NULL || length == 0U || length >= UMI_AI_WORKER_JSON_LIMIT || length > INT_MAX ||
        !WorkerUtf8((const unsigned char *)text, length)) return UMI_STATUS_PARSE_ERROR;
    size_t asciiLength = 0U;
    char *ascii = WorkerAsciiJson(text, length, &asciiLength);
    if (ascii == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    json_tokener *parser = json_tokener_new_ex(48);
    if (parser == NULL) { free(ascii); return UMI_STATUS_OUT_OF_MEMORY; }
    json_tokener_set_flags(parser, JSON_TOKENER_STRICT);
    json_object *value = json_tokener_parse_ex(parser, ascii, (int)asciiLength);
    bool valid = json_tokener_get_error(parser) == json_tokener_success && json_object_is_type(value, json_type_object);
    for (size_t i = json_tokener_get_parse_end(parser); valid && i < asciiLength; ++i)
        if (ascii[i] != ' ' && ascii[i] != '\t' && ascii[i] != '\r' && ascii[i] != '\n') valid = false;
    json_tokener_free(parser); free(ascii);
    if (!valid) { if (value != NULL) json_object_put(value); return UMI_STATUS_PARSE_ERROR; }
    *out = value;
    return UMI_STATUS_OK;
}

/* Read a string without accepting embedded JSON NULs or implicit conversions. */
static const char *WorkerString(json_object *root, const char *name, size_t limit)
{
    json_object *value = NULL;
    if (!json_object_object_get_ex(root, name, &value) || !json_object_is_type(value, json_type_string)) return NULL;
    const char *text = json_object_get_string(value);
    int n = json_object_get_string_len(value);
    if (text == NULL || n < 0 || (size_t)n > limit || strlen(text) != (size_t)n) return NULL;
    return text;
}

/* IDs become URL segments, so delimiters, percent escapes and control bytes are forbidden. */
static bool WorkerId(const char *id)
{
    if (id == NULL || *id == '\0' || strlen(id) > 128U) return false;
    for (const unsigned char *s = (const unsigned char *)id; *s != 0U; ++s)
        if (!(isalnum(*s) || *s == '-' || *s == '_')) return false;
    return true;
}

/* File names refer only to worker-owned relative assets; they never choose a host. */
static bool WorkerRelative(const char *name, bool allowEmpty)
{
    if (name == NULL || strlen(name) > 1024U || (!allowEmpty && *name == '\0') || *name == '/') return false;
    const char *part = name;
    for (const unsigned char *s = (const unsigned char *)name;; ++s) {
        if (*s == '\\' || *s == ':' || (*s != 0U && *s < 32U) || *s == 127U) return false;
        if (*s == '/' || *s == 0U) {
            size_t n = (size_t)((const char *)s - part);
            if ((n == 0U && *name != '\0') || (n == 1U && part[0] == '.') ||
                (n == 2U && part[0] == '.' && part[1] == '.')) return false;
            if (*s == 0U) break;
            part = (const char *)s + 1;
        }
    }
    return true;
}

/* Overflow checks happen before multiplication, allocation or caller callbacks. */
static size_t WorkerWrite(char *data, size_t size, size_t count, void *context)
{
    WorkerReply *reply = context;
    if (size != 0U && count > SIZE_MAX / size) { reply->overflow = true; return 0U; }
    size_t bytes = size * count;
    if (bytes > reply->limit - reply->length) { reply->overflow = true; return 0U; }
    if (reply->sink != NULL) {
        if (!reply->sink(reply->context, data, bytes)) { reply->sinkFailed = true; return 0U; }
    } else {
        char *next = realloc(reply->data, reply->length + bytes + 1U);
        if (next == NULL) { reply->sinkFailed = true; return 0U; }
        reply->data = next;
        memcpy(reply->data + reply->length, data, bytes);
        reply->data[reply->length + bytes] = '\0';
    }
    reply->length += bytes;
    return bytes;
}

/* Cancellation is a caller-owned signal, not a global kill of other worker jobs. */
static int WorkerProgress(void *context, curl_off_t dt, curl_off_t dn, curl_off_t ut, curl_off_t un)
{
    const UmiAiWorkerEndpoint *endpoint = context;
    (void)dt; (void)dn; (void)ut; (void)un;
    return endpoint->cancelled != NULL && endpoint->cancelled(endpoint->cancellationContext) ? 1 : 0;
}

/* Fixed paths and a numeric local port prevent the caller from supplying arbitrary URLs. */
static UmiStatus WorkerHttp(const UmiAiWorkerEndpoint *endpoint, const char *path,
    const char *body, WorkerReply *reply)
{
    char url[4096];
    int length = snprintf(url, sizeof(url), "http://127.0.0.1:%u%s", (unsigned)endpoint->port, path);
    if (length < 0 || (size_t)length >= sizeof(url)) return UMI_STATUS_CAPACITY_EXCEEDED;
    CURL *curl = curl_easy_init();
    if (curl == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    struct curl_slist *headers = curl_slist_append(NULL, "Content-Type: application/json");
    UmiStatus status = UMI_STATUS_UNAVAILABLE;
    if (headers == NULL) { curl_easy_cleanup(curl); return UMI_STATUS_OUT_OF_MEMORY; }
    /* Every security-relevant option is checked; redirects and ambient proxies are off. */
#define SET(option, value) do { if (curl_easy_setopt(curl, option, value) != CURLE_OK) goto done; } while (0)
    SET(CURLOPT_URL, url); SET(CURLOPT_PROXY, ""); SET(CURLOPT_NOPROXY, "*");
    SET(CURLOPT_NETRC, (long)CURL_NETRC_IGNORED); SET(CURLOPT_FOLLOWLOCATION, 0L);
    SET(CURLOPT_MAXREDIRS, 0L); SET(CURLOPT_PROTOCOLS_STR, "http"); SET(CURLOPT_REDIR_PROTOCOLS_STR, "http");
    SET(CURLOPT_NOSIGNAL, 1L); SET(CURLOPT_FAILONERROR, 1L); SET(CURLOPT_HTTPHEADER, headers);
    SET(CURLOPT_CONNECTTIMEOUT_MS, 3000L); SET(CURLOPT_TIMEOUT_MS, (long)endpoint->timeoutMillis);
    SET(CURLOPT_WRITEFUNCTION, WorkerWrite); SET(CURLOPT_WRITEDATA, reply);
    SET(CURLOPT_NOPROGRESS, 0L); SET(CURLOPT_XFERINFOFUNCTION, WorkerProgress);
    SET(CURLOPT_XFERINFODATA, endpoint);
    if (body != NULL) {
        SET(CURLOPT_POST, 1L); SET(CURLOPT_POSTFIELDS, body);
        SET(CURLOPT_POSTFIELDSIZE_LARGE, (curl_off_t)strlen(body));
    }
    CURLcode result = curl_easy_perform(curl);
    long code = 0L;
    (void)curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &code);
    if (reply->overflow) status = UMI_STATUS_CAPACITY_EXCEEDED;
    else if (result == CURLE_OPERATION_TIMEDOUT) status = UMI_STATUS_TIMEOUT;
    else if (result == CURLE_ABORTED_BY_CALLBACK) status = UMI_STATUS_CANCELLED;
    else if (code == 429L || code == 503L) status = UMI_STATUS_BUSY;
    else if (result != CURLE_OK || code != 200L) status = UMI_STATUS_IO_ERROR;
    else status = UMI_STATUS_OK;
 done:
    curl_slist_free_all(headers); curl_easy_cleanup(curl);
    return status;
#undef SET
}

/* The allow-list deliberately excludes custom code, cloud API nodes and URL loaders. */
static bool WorkerClassAllowed(const char *name)
{
    static const char *const classes[] = {
        "CheckpointLoaderSimple", "CLIPTextEncode", "EmptyLatentImage", "KSampler",
        "KSamplerAdvanced", "VAEDecode", "VAEDecodeTiled", "VAEEncode", "VAEEncodeTiled",
        "SaveImage", "PreviewImage", "UNETLoader", "CLIPLoader", "DualCLIPLoader", "VAELoader",
        "EmptySD3LatentImage", "EmptyFlux2LatentImage", "ModelSamplingSD3", "ModelSamplingFlux",
        "ModelSamplingAuraFlow", "FluxGuidance", "ConditioningZeroOut", "ConditioningSetTimestepRange",
        "ConditioningCombine", "EmptyHunyuanLatentVideo", "WanImageToVideo", "Wan22ImageToVideoLatent",
        "WanFirstLastFrameToVideo", "CreateVideo", "SaveVideo", "SaveAnimatedWEBP", "LoadImage",
        "ImageScale", "ImageScaleBy", "LoraLoader", "LoraLoaderModelOnly", "RandomNoise",
        "BasicGuider", "CFGGuider", "KSamplerSelect", "BasicScheduler", "SamplerCustomAdvanced"
    };
    for (size_t i = 0U; i < sizeof(classes)/sizeof(classes[0]); ++i)
        if (strcmp(name, classes[i]) == 0) return true;
    return false;
}

/* Validate every node before the first network request; outputs get a fixed safe prefix. */
static UmiStatus WorkerWorkflow(json_object *graph, const char *client)
{
    if (!json_object_is_type(graph, json_type_object) || json_object_object_length(graph) < 1 ||
        json_object_object_length(graph) > 256 || !WorkerId(client)) return UMI_STATUS_INVALID_ARGUMENT;
    json_object_object_foreach(graph, key, node) {
        if (!WorkerId(key) || !json_object_is_type(node, json_type_object)) return UMI_STATUS_INVALID_ARGUMENT;
        const char *className = WorkerString(node, "class_type", 96U);
        json_object *inputs = NULL;
        if (className == NULL || !WorkerClassAllowed(className) ||
            !json_object_object_get_ex(node, "inputs", &inputs) || !json_object_is_type(inputs, json_type_object))
            return UMI_STATUS_PERMISSION_DENIED;
        static const char *const files[] = {"ckpt_name", "unet_name", "clip_name", "clip_name1", "clip_name2",
                                             "vae_name", "lora_name", "image"};
        for (size_t j = 0U; j < sizeof(files)/sizeof(files[0]); ++j) {
            json_object *value = NULL;
            if (json_object_object_get_ex(inputs, files[j], &value) && json_object_is_type(value, json_type_string)) {
                const char *name = WorkerString(inputs, files[j], 1024U);
                if (!WorkerRelative(name, false)) return UMI_STATUS_PERMISSION_DENIED;
            }
        }
        /* Prefix mutation applies only to disk-output nodes, never user prompt strings. */
        if (strcmp(className, "SaveImage") == 0 || strcmp(className, "SaveVideo") == 0 ||
            strcmp(className, "SaveAnimatedWEBP") == 0) {
            char prefix[160]; (void)snprintf(prefix, sizeof(prefix), "umicom/%s", client);
            json_object *value = json_object_new_string(prefix);
            if (value == NULL) return UMI_STATUS_OUT_OF_MEMORY;
            if (json_object_object_add(inputs, "filename_prefix", value) != 0) {
                json_object_put(value); return UMI_STATUS_OUT_OF_MEMORY;
            }
        }
    }
    return UMI_STATUS_OK;
}

/* Keep integer configuration bounded and reject floating point values and booleans. */
static bool WorkerInteger(json_object *root, const char *key, int64_t low, int64_t high, int64_t *out)
{
    json_object *value = NULL;
    if (!json_object_object_get_ex(root, key, &value) || !json_object_is_type(value, json_type_int)) return false;
    int64_t result = json_object_get_int64(value);
    if (result < low || result > high) return false;
    *out = result; return true;
}

/* Replace existing template slots using JSON constructors, never string interpolation. */
static bool WorkerSet(json_object *root, const char *node, const char *key, json_object *value)
{
    json_object *entry = NULL, *inputs = NULL;
    if (value == NULL) return false;
    if (!json_object_object_get_ex(root, node, &entry) || !json_object_object_get_ex(entry, "inputs", &inputs) ||
        json_object_object_add(inputs, key, value) != 0) { json_object_put(value); return false; }
    return true;
}

/* A minimal checkpoint workflow supports SD 1.x and SDXL base without custom nodes. */
static UmiStatus WorkerImageGraph(json_object *request, json_object **out)
{
    const char *checkpoint = WorkerString(request, "checkpoint", 1024U);
    const char *prompt = WorkerString(request, "prompt", 16000U);
    const char *negative = WorkerString(request, "negative", 16000U);
    int64_t width, height, seed, steps;
    if (!WorkerRelative(checkpoint, false) || prompt == NULL || *prompt == '\0' || negative == NULL ||
        !WorkerInteger(request, "width", 256, 1536, &width) || width % 8 != 0 ||
        !WorkerInteger(request, "height", 256, 1536, &height) || height % 8 != 0 ||
        !WorkerInteger(request, "steps", 1, 80, &steps) ||
        !WorkerInteger(request, "seed", 0, INT64_C(9007199254740991), &seed)) return UMI_STATUS_INVALID_ARGUMENT;
    static const char graph[] =
        "{\"3\":{\"class_type\":\"KSampler\",\"inputs\":{\"model\":[\"4\",0],\"positive\":[\"6\",0],\"negative\":[\"7\",0],\"latent_image\":[\"5\",0],\"seed\":0,\"steps\":20,\"cfg\":7,\"sampler_name\":\"euler\",\"scheduler\":\"normal\",\"denoise\":1}},"
        "\"4\":{\"class_type\":\"CheckpointLoaderSimple\",\"inputs\":{\"ckpt_name\":\"\"}},"
        "\"5\":{\"class_type\":\"EmptyLatentImage\",\"inputs\":{\"width\":512,\"height\":512,\"batch_size\":1}},"
        "\"6\":{\"class_type\":\"CLIPTextEncode\",\"inputs\":{\"text\":\"\",\"clip\":[\"4\",1]}},"
        "\"7\":{\"class_type\":\"CLIPTextEncode\",\"inputs\":{\"text\":\"\",\"clip\":[\"4\",1]}},"
        "\"8\":{\"class_type\":\"VAEDecode\",\"inputs\":{\"samples\":[\"3\",0],\"vae\":[\"4\",2]}},"
        "\"9\":{\"class_type\":\"SaveImage\",\"inputs\":{\"images\":[\"8\",0],\"filename_prefix\":\"umicom\"}}}";
    UmiStatus status = UmiAiWorkerParseJson(graph, sizeof(graph)-1U, out);
    if (status != UMI_STATUS_OK) return status;
    if (!WorkerSet(*out, "4", "ckpt_name", json_object_new_string(checkpoint)) ||
        !WorkerSet(*out, "6", "text", json_object_new_string(prompt)) ||
        !WorkerSet(*out, "7", "text", json_object_new_string(negative)) ||
        !WorkerSet(*out, "5", "width", json_object_new_int64(width)) ||
        !WorkerSet(*out, "5", "height", json_object_new_int64(height)) ||
        !WorkerSet(*out, "3", "seed", json_object_new_int64(seed)) ||
        !WorkerSet(*out, "3", "steps", json_object_new_int64(steps))) {
        json_object_put(*out); *out = NULL; return UMI_STATUS_OUT_OF_MEMORY;
    }
    return UMI_STATUS_OK;
}

/* Derive an output solely from the requested job's history, not an arbitrary path. */
static UmiStatus WorkerOutput(const UmiAiWorkerEndpoint *endpoint, json_object *request,
    UmiAiWorkerWrite write, void *context)
{
    const char *id = WorkerString(request, "prompt_id", 128U);
    const char *nodeId = WorkerString(request, "node_id", 128U);
    const char *group = WorkerString(request, "group", 16U);
    int64_t index;
    if (!WorkerId(id) || !WorkerId(nodeId) || group == NULL ||
        (strcmp(group, "images") && strcmp(group, "videos") && strcmp(group, "gifs")) ||
        !WorkerInteger(request, "index", 0, 1023, &index)) return UMI_STATUS_INVALID_ARGUMENT;
    char path[4096]; (void)snprintf(path, sizeof(path), "/history/%s", id);
    WorkerReply history = {.limit=UMI_AI_WORKER_JSON_LIMIT-1U};
    UmiStatus status = WorkerHttp(endpoint, path, NULL, &history);
    json_object *root = NULL, *job = NULL, *outputs = NULL, *node = NULL, *items = NULL;
    if (status != UMI_STATUS_OK) goto done;
    status = UmiAiWorkerParseJson(history.data, history.length, &root);
    if (status != UMI_STATUS_OK) goto done;
    status = UMI_STATUS_NOT_FOUND;
    if (!json_object_object_get_ex(root, id, &job) || !json_object_is_type(job, json_type_object) ||
        !json_object_object_get_ex(job, "outputs", &outputs) || !json_object_is_type(outputs, json_type_object) ||
        !json_object_object_get_ex(outputs, nodeId, &node) || !json_object_is_type(node, json_type_object) ||
        !json_object_object_get_ex(node, group, &items) || !json_object_is_type(items, json_type_array) ||
        (size_t)index >= json_object_array_length(items)) goto done;
    json_object *item = json_object_array_get_idx(items, (size_t)index);
    if (!json_object_is_type(item, json_type_object)) { status=UMI_STATUS_PARSE_ERROR; goto done; }
    const char *filename = WorkerString(item, "filename", 1024U);
    const char *subfolder = WorkerString(item, "subfolder", 1024U);
    const char *type = WorkerString(item, "type", 16U);
    status = UMI_STATUS_PERMISSION_DENIED;
    if (!WorkerRelative(filename, false) || strchr(filename, '/') != NULL ||
        !WorkerRelative(subfolder, true) || type == NULL || strcmp(type, "output") != 0) goto done;
    const char *extension = strrchr(filename, '.');
    if (extension == NULL || (strcmp(extension,".png") && strcmp(extension,".jpg") &&
        strcmp(extension,".jpeg") && strcmp(extension,".webp") && strcmp(extension,".gif") &&
        strcmp(extension,".mp4") && strcmp(extension,".webm"))) goto done;
    char *nameEscaped = curl_easy_escape(NULL, filename, 0);
    char *folderEscaped = curl_easy_escape(NULL, subfolder, 0);
    if (nameEscaped == NULL || folderEscaped == NULL) {
        curl_free(nameEscaped); curl_free(folderEscaped); status=UMI_STATUS_OUT_OF_MEMORY; goto done;
    }
    int n = snprintf(path, sizeof(path), "/view?filename=%s&subfolder=%s&type=output", nameEscaped, folderEscaped);
    curl_free(nameEscaped); curl_free(folderEscaped);
    if (n < 0 || (size_t)n >= sizeof(path)) { status=UMI_STATUS_CAPACITY_EXCEEDED; goto done; }
    WorkerReply media = {.limit=UMI_AI_WORKER_MEDIA_LIMIT, .sink=write, .context=context};
    status = WorkerHttp(endpoint, path, NULL, &media);
    if (status == UMI_STATUS_OK && media.length == 0U) status=UMI_STATUS_PARSE_ERROR;
 done:
    if (root != NULL) json_object_put(root);
    free(history.data); return status;
}

/* One call performs one approved action; a timeout never triggers a hidden retry. */
UmiStatus UmiAiWorkerExecute(const UmiAiWorkerEndpoint *endpoint,
    const char *json, size_t length, UmiAiWorkerWrite write, void *writeContext,
    char *error, size_t errorCapacity)
{
    if (error != NULL && errorCapacity > 0U) error[0] = '\0';
    if (endpoint == NULL || endpoint->port < 1024U || endpoint->timeoutMillis < 100U ||
        endpoint->timeoutMillis > 60000U || write == NULL)
        return WorkerError(UMI_STATUS_INVALID_ARGUMENT,error,errorCapacity,"Invalid worker port, timeout or sink.");
    json_object *request=NULL, *body=NULL, *graph=NULL;
    UmiStatus status = UmiAiWorkerParseJson(json,length,&request);
    if (status != UMI_STATUS_OK) return WorkerError(status,error,errorCapacity,"Expected one bounded JSON object.");
    const char *action=WorkerString(request,"action",40U);
    char path[256]="";
    const char *send=NULL;
    WorkerReply reply={.limit=UMI_AI_WORKER_JSON_LIMIT-1U};
    bool curlReady=false, submitted=false;
    if (action == NULL) { status=UMI_STATUS_INVALID_ARGUMENT; goto done; }
    if (strcmp(action,"media-info")==0) (void)snprintf(path,sizeof(path),"/system_stats");
    else if (strcmp(action,"media-checkpoints")==0) (void)snprintf(path,sizeof(path),"/object_info/CheckpointLoaderSimple");
    else if (strcmp(action,"text-models")==0) (void)snprintf(path,sizeof(path),"/v1/models");
    else if (strcmp(action,"text-tokenize")==0) {
        const char *text=WorkerString(request,"text",65536U);
        if (text==NULL) { status=UMI_STATUS_INVALID_ARGUMENT; goto done; }
        body=json_object_new_object();
        if(body==NULL) {status=UMI_STATUS_OUT_OF_MEMORY;goto done;}
        json_object_object_add(body,"content",json_object_new_string(text));
        json_object_object_add(body,"add_special",json_object_new_boolean(0));
        (void)snprintf(path,sizeof(path),"/tokenize");
    } else if (strcmp(action,"media-status")==0) {
        const char *id=WorkerString(request,"prompt_id",128U);
        if (!WorkerId(id)) {status=UMI_STATUS_INVALID_ARGUMENT;goto done;}
        (void)snprintf(path,sizeof(path),"/history/%s",id);
    } else if (strcmp(action,"media-image")==0 || strcmp(action,"media-submit")==0) {
        json_object *approval=NULL;
        const char *client=WorkerString(request,"client_id",128U);
        if(!json_object_object_get_ex(request,"approved",&approval) ||
           !json_object_is_type(approval,json_type_boolean) || !json_object_get_boolean(approval) || !WorkerId(client)) {
            status=UMI_STATUS_PERMISSION_DENIED;goto done;
        }
        if(strcmp(action,"media-image")==0) status=WorkerImageGraph(request,&graph);
        else {
            json_object *original=NULL;
            if(!json_object_object_get_ex(request,"workflow",&original) || !json_object_is_type(original,json_type_object)) {
                status=UMI_STATUS_INVALID_ARGUMENT;goto done;
            }
            graph=json_object_get(original); status=UMI_STATUS_OK;
        }
        if(status!=UMI_STATUS_OK) goto done;
        status=WorkerWorkflow(graph,client); if(status!=UMI_STATUS_OK) goto done;
        body=json_object_new_object(); if(body==NULL) {status=UMI_STATUS_OUT_OF_MEMORY;goto done;}
        json_object_object_add(body,"prompt",graph);graph=NULL;
        json_object_object_add(body,"client_id",json_object_new_string(client));
        (void)snprintf(path,sizeof(path),"/prompt"); submitted=true;
    } else if(strcmp(action,"media-output")!=0) {status=UMI_STATUS_PERMISSION_DENIED;goto done;}
    if(curl_global_init(CURL_GLOBAL_DEFAULT)!=CURLE_OK) {status=UMI_STATUS_UNAVAILABLE;goto done;}
    curlReady=true;
    if(strcmp(action,"media-output")==0) {status=WorkerOutput(endpoint,request,write,writeContext);goto done;}
    if(body!=NULL) send=json_object_to_json_string_ext(body,JSON_C_TO_STRING_PLAIN);
    status=WorkerHttp(endpoint,path,send,&reply);
    if(status==UMI_STATUS_OK) {
        json_object *parsed=NULL;
        status=UmiAiWorkerParseJson(reply.data,reply.length,&parsed);
        if(status==UMI_STATUS_OK) {
            json_object *backendError=NULL;
            if(json_object_object_get_ex(parsed,"error",&backendError)) status=UMI_STATUS_INVALID_STATE;
            if(submitted && !WorkerId(WorkerString(parsed,"prompt_id",128U))) status=UMI_STATUS_PARSE_ERROR;
            if(status==UMI_STATUS_OK && !write(writeContext,reply.data,reply.length)) status=UMI_STATUS_IO_ERROR;
            json_object_put(parsed);
        }
    }
 done:
    if(graph!=NULL) json_object_put(graph);
    if(body!=NULL) json_object_put(body);
    if(request!=NULL) json_object_put(request);
    free(reply.data);
    if(curlReady) curl_global_cleanup();
    if(status!=UMI_STATUS_OK) return WorkerError(status,error,errorCapacity,
        submitted ? "Submission was not acknowledged. It may already be queued; inspect the worker before retrying." :
        "Worker request rejected or failed. Check the approved input, tunnel, worker logs and supported operation.");
    return status;
}
