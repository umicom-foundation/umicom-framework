/*-----------------------------------------------------------------------------
 * Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
 * Native Windows removable-USB observation and gated acquisition. System-volume
 * extents are excluded; every target volume must be single-disk and lockable.
 * Failed identity/extent/lock checks occur before the first disk write.
 *---------------------------------------------------------------------------*/

#include "internal.h"
#include <winioctl.h>
#include <wchar.h>
#include <inttypes.h>
static int DiskPath(const char *p,unsigned *number)
{
    static const char prefix[]="\\\\.\\PhysicalDrive";
    if (!p||strncmp(p,prefix,sizeof prefix-1U))return 0;
    const char *s=p+sizeof prefix-1U;
    if (!*s||(s[0]=='0'&&s[1]))return 0;
    unsigned v=0;
    for (;*s;++s) {
        if (*s<'0'||*s>'9'||v>25U)return 0;
        v=v*10U+(unsigned)(*s-'0');
    }
    if (v>255U)return 0;
    *number=v;
    return 1;
}

static int Field(const unsigned char *buffer,size_t bytes,DWORD offset,char *out,size_t cap)
{
    if (!offset||offset>=bytes)return 0;
    size_t n=0;
    while ((size_t)offset+n<bytes&&buffer[offset+n]) {
        if (n+1U>=cap||buffer[offset+n]<32U||buffer[offset+n]>126U)return 0;
        out[n]=(char)buffer[offset+n];
        ++n;
    }
    if ((size_t)offset+n==bytes)return 0;
    while (n&&out[n-1U]==' ')--n;
    out[n]=0;
    return n!=0;
}

static UmiStatus Observe(HANDLE h,unsigned number,UmiBootMediaDevice *out)
{
    _Alignas(8) unsigned char data[4096];
    DWORD got=0;
    STORAGE_PROPERTY_QUERY query;
    memset(&query,0,sizeof query);
    query.PropertyId=StorageDeviceProperty;
    query.QueryType=PropertyStandardQuery;
    if (!DeviceIoControl(h,IOCTL_STORAGE_QUERY_PROPERTY,&query,sizeof query,data,sizeof data,&got,NULL)||got<sizeof(STORAGE_DEVICE_DESCRIPTOR))return UMI_STATUS_UNAVAILABLE;
    const STORAGE_DEVICE_DESCRIPTOR*d=(const STORAGE_DEVICE_DESCRIPTOR*)data;
    if (d->Size>got||d->Size<sizeof *d)return UMI_STATUS_PARSE_ERROR;
    out->usb=d->BusType==BusTypeUsb;
    out->removable=d->RemovableMedia!=0;
    if (!out->usb||!out->removable)return UMI_STATUS_PERMISSION_DENIED;
    if (!Field(data,d->Size,d->ProductIdOffset,out->model,sizeof out->model)||!Field(data,d->Size,d->SerialNumberOffset,
        out->serial,sizeof out->serial))return UMI_STATUS_UNAVAILABLE;
    DISK_GEOMETRY_EX geometry;
    STORAGE_DEVICE_NUMBER id;
    GET_LENGTH_INFORMATION length;
    memset(&geometry,0,sizeof geometry);
    memset(&id,0,sizeof id);
    memset(&length,0,sizeof length);
    /* Each native reply has its own returned-size check. A partial reply must
     * not turn uninitialised geometry into permission to open a physical disk. */
    if (!DeviceIoControl(h,IOCTL_DISK_GET_DRIVE_GEOMETRY_EX,NULL,0,&geometry,sizeof geometry,&got,NULL)||
        (size_t)got<(size_t)FIELD_OFFSET(DISK_GEOMETRY_EX,Data))return UMI_STATUS_UNAVAILABLE;
    if (!DeviceIoControl(h,IOCTL_DISK_GET_LENGTH_INFO,NULL,0,&length,sizeof length,&got,NULL)||
        got<sizeof length)return UMI_STATUS_UNAVAILABLE;
    if (!DeviceIoControl(h,IOCTL_STORAGE_GET_DEVICE_NUMBER,NULL,0,&id,sizeof id,&got,NULL)||
        got<sizeof id||id.DeviceNumber!=number||id.DeviceType!=FILE_DEVICE_DISK||
        length.Length.QuadPart<=0)return UMI_STATUS_UNAVAILABLE;
    out->bytes=(uint64_t)length.Length.QuadPart;
    out->sectorBytes=geometry.Geometry.BytesPerSector;

    /* A drive number can be reused after unplugging. Bind it to the native
     * hardware-derived GUID as well as the observed serial and geometry. An
     * older SDK/OS or ambiguous hardware identity is not a write candidate. */

#ifdef IOCTL_STORAGE_GET_DEVICE_NUMBER_EX
    STORAGE_DEVICE_NUMBER_EX extended;
    memset(&extended, 0, sizeof extended);
    if (!DeviceIoControl(h,IOCTL_STORAGE_GET_DEVICE_NUMBER_EX,NULL,0,
        &extended,sizeof extended,&got,NULL) || got < sizeof extended ||
        extended.Size < sizeof extended || extended.DeviceType != FILE_DEVICE_DISK ||
        extended.DeviceNumber != number ||
        (extended.Flags & (STORAGE_DEVICE_FLAGS_RANDOM_DEVICEGUID_REASON_CONFLICT |
        STORAGE_DEVICE_FLAGS_RANDOM_DEVICEGUID_REASON_NOHWID)))return UMI_STATUS_UNAVAILABLE;
    const unsigned char *guid = (const unsigned char *)&extended.DeviceGuid;
    char identity[33];
    static const char digits[] = "0123456789abcdef";
    unsigned nonzero = 0;
    for (size_t i=0;i<16U;++i) {
        nonzero |= guid[i];
        identity[2U*i]=digits[guid[i]>>4];
        identity[2U*i+1U]=digits[guid[i]&15U];
    }
    identity[32]=0;
    if (!nonzero)return UMI_STATUS_UNAVAILABLE;
    int n=snprintf(out->identity,sizeof out->identity,"windows:%u:%s:%.127s:%" PRIu64 ":%u",number,identity,
        out->serial,out->bytes,out->sectorBytes);
    if (n<0||(size_t)n>=sizeof out->identity)return UMI_STATUS_CAPACITY_EXCEEDED;

#else
    return UMI_STATUS_UNAVAILABLE;

#endif
    GET_DISK_ATTRIBUTES attributes;
    memset(&attributes,0,sizeof attributes);
    attributes.Version=sizeof attributes;
    if (DeviceIoControl(h,IOCTL_DISK_GET_DISK_ATTRIBUTES,NULL,0,&attributes,sizeof attributes,&got,NULL))out->readOnly=(attributes.Attributes&DISK_ATTRIBUTE_READ_ONLY)!=0;

    /* This read-only control query must give a definite writable/write-protected
     * answer. Missing attribute support is not converted into "writable". */
    if(!DeviceIoControl(h,IOCTL_DISK_IS_WRITABLE,NULL,0,NULL,0,&got,NULL)){
        if(GetLastError()!=ERROR_WRITE_PROTECT)return UMI_STATUS_UNAVAILABLE;
        out->readOnly=1;
    }
    return UMI_STATUS_OK;
}

static UmiStatus Volumes(unsigned number,BmFile *locked,int *system)
{
    wchar_t windows[4096],mount[4096],systemVolume[4096],name[4096];
    *system=0;
    UINT windowsLength=GetWindowsDirectoryW(windows,4096);
    if (!windowsLength||windowsLength>=4096U||!GetVolumePathNameW(windows,mount,4096)||
        !GetVolumeNameForVolumeMountPointW(mount,systemVolume,4096))return UMI_STATUS_UNAVAILABLE;
    HANDLE find=FindFirstVolumeW(name,4096);
    if (find==INVALID_HANDLE_VALUE)return UMI_STATUS_UNAVAILABLE;
    UmiStatus status=UMI_STATUS_OK;
    BOOL more=TRUE;
    while (more) {
        size_t len=wcslen(name);
        if (!len||name[len-1U]!=L'\\') {
            status=UMI_STATUS_PARSE_ERROR;
            break;
        }
        wchar_t path[4096];
        memcpy(path,name,(len+1U)*sizeof *path);
        path[len-1U]=0;
        HANDLE volume=CreateFileW(path,0,FILE_SHARE_READ|FILE_SHARE_WRITE,NULL,OPEN_EXISTING,0,NULL);
        if (volume==INVALID_HANDLE_VALUE) {
            status=UMI_STATUS_PERMISSION_DENIED;
            break;
        }
        _Alignas(8) unsigned char buffer[sizeof(VOLUME_DISK_EXTENTS)+64U*sizeof(DISK_EXTENT)];
        DWORD got=0;
        BOOL known=DeviceIoControl(volume,IOCTL_VOLUME_GET_VOLUME_DISK_EXTENTS,NULL,0,buffer,sizeof buffer,&got,
            NULL);
        if (!known) {
            DWORD error=GetLastError();
            CloseHandle(volume);

            /* Optical volumes have no disk extents. Unknown fixed/removable
             * volumes are never ignored, even if this means refusing a write. */
            UINT kind=GetDriveTypeW(name);
            if (kind!=DRIVE_CDROM||(error!=ERROR_INVALID_FUNCTION&&error!=ERROR_NOT_SUPPORTED)) {
                status=UMI_STATUS_UNAVAILABLE;
                break;
            }
        }
        else {
            VOLUME_DISK_EXTENTS *e=(VOLUME_DISK_EXTENTS*)buffer;
            if (got<sizeof(VOLUME_DISK_EXTENTS)||!e->NumberOfDiskExtents||e->NumberOfDiskExtents>64U||
                (size_t)e->NumberOfDiskExtents>((size_t)got-(size_t)FIELD_OFFSET(VOLUME_DISK_EXTENTS,Extents))/sizeof(DISK_EXTENT)) {
                CloseHandle(volume);
                status=UMI_STATUS_PARSE_ERROR;
                break;
            }
            int target=0;
            for (DWORD i=0;i<e->NumberOfDiskExtents;++i)if (e->Extents[i].DiskNumber==number)target=1;
            CloseHandle(volume);
            if (target) {
                if (!_wcsicmp(name,systemVolume)) {
                    *system=1;
                    status=UMI_STATUS_PERMISSION_DENIED;
                    break;
                }
                if (e->NumberOfDiskExtents!=1U) {
                    status=UMI_STATUS_PERMISSION_DENIED;
                    break;
                }
                if (locked) {
                    if (locked->volumeCount==64U) {
                        status=UMI_STATUS_CAPACITY_EXCEEDED;
                        break;
                    }
                    volume=CreateFileW(path,GENERIC_READ|GENERIC_WRITE,FILE_SHARE_READ|FILE_SHARE_WRITE,NULL,OPEN_EXISTING,
                        0,NULL);
                    if (volume==INVALID_HANDLE_VALUE) {
                        status=UMI_STATUS_PERMISSION_DENIED;
                        break;
                    }
                    if (!DeviceIoControl(volume,FSCTL_LOCK_VOLUME,NULL,0,NULL,0,&got,NULL)) {
                        CloseHandle(volume);
                        status=UMI_STATUS_PERMISSION_DENIED;
                        break;
                    }
                    locked->volumes[locked->volumeCount++]=volume;
                    if (!DeviceIoControl(volume,FSCTL_DISMOUNT_VOLUME,NULL,0,NULL,0,&got,NULL)) {
                        status=UMI_STATUS_IO_ERROR;
                        break;
                    }
                }
            }
        }
        more=FindNextVolumeW(find,name,4096);
        if (!more&&GetLastError()!=ERROR_NO_MORE_FILES)status=UMI_STATUS_IO_ERROR;
    }
    FindVolumeClose(find);
    return status;
}

UmiStatus BmDeviceCheck(const char *path,UmiBootMediaDevice *out)
{
    unsigned number;
    if (!out||!DiskPath(path,&number))return UMI_STATUS_INVALID_ARGUMENT;
    memset(out,0,sizeof *out);
    strcpy(out->path,path);
    wchar_t wide[64];
    if (!MultiByteToWideChar(CP_UTF8,MB_ERR_INVALID_CHARS,path,-1,wide,64))return UMI_STATUS_INVALID_ARGUMENT;
    HANDLE h=CreateFileW(wide,0,FILE_SHARE_READ|FILE_SHARE_WRITE,NULL,OPEN_EXISTING,0,NULL);
    if (h==INVALID_HANDLE_VALUE)return GetLastError()==ERROR_FILE_NOT_FOUND?UMI_STATUS_NOT_FOUND:UMI_STATUS_PERMISSION_DENIED;
    UmiStatus s=Observe(h,number,out);
    CloseHandle(h);
    if (s!=UMI_STATUS_OK)return s;
    int system=0;
    s=Volumes(number,NULL,&system);
    out->protectedDevice=system||s!=UMI_STATUS_OK;
    if (system)strcpy(out->reason,"Contains the running Windows volume.");
    else if (s!=UMI_STATUS_OK)strcpy(out->reason,"Cannot completely establish volume ownership; writing is blocked.");
    else if (out->readOnly)strcpy(out->reason,"Disk is read-only.");
    else strcpy(out->reason,"All target volumes must still accept exclusive locks at execution.");
    return UMI_STATUS_OK;
}

UmiStatus UmiBootMediaDiscover(UmiBootMediaInventory *out,UmiBootMediaReport *r)
{
    if (!out)return UMI_STATUS_INVALID_ARGUMENT;
    memset(out,0,sizeof *out);
    for (unsigned i=0;i<256U;++i) {
        char path[64];
        (void)snprintf(path,sizeof path,"\\\\.\\PhysicalDrive%u",i);
        UmiBootMediaDevice item;
        UmiStatus s=BmDeviceCheck(path,&item);
        if (s!=UMI_STATUS_OK)continue;
        if (out->count==UMI_BOOT_MEDIA_DEVICES) {
            ++out->omitted;
            continue;
        }
        out->devices[out->count++]=item;
    }
    return BmMessage(r,out->omitted?UMI_STATUS_CAPACITY_EXCEEDED:UMI_STATUS_OK,"Read-only inventory: %zu identified removable USB disks. Unknown devices are not write candidates.",
        out->count);
}

UmiStatus BmDeviceOpen(const UmiBootMediaDevice *expected,int write,BmFile **out)
{
    unsigned number;
    if (!out||!expected||!DiskPath(expected->path,&number))return UMI_STATUS_INVALID_ARGUMENT;
    *out=NULL;
    if (write&&!UmiBootMediaDeviceWritesEnabled())return UMI_STATUS_UNAVAILABLE;
    UmiBootMediaDevice observed;
    UmiStatus s=BmDeviceCheck(expected->path,&observed);
    if (s!=UMI_STATUS_OK)return s;
    if (!BmSameDevice(expected,&observed)||observed.protectedDevice||observed.readOnly)return UMI_STATUS_PERMISSION_DENIED;
    wchar_t path[64];
    if (!MultiByteToWideChar(CP_UTF8,MB_ERR_INVALID_CHARS,expected->path,-1,path,64))return UMI_STATUS_INVALID_ARGUMENT;
    HANDLE h=CreateFileW(path,write?(GENERIC_READ|GENERIC_WRITE):GENERIC_READ,FILE_SHARE_READ|FILE_SHARE_WRITE,
        NULL,OPEN_EXISTING,FILE_FLAG_WRITE_THROUGH,NULL);
    if (h==INVALID_HANDLE_VALUE)return UMI_STATUS_PERMISSION_DENIED;
    BmFile*f=calloc(1,sizeof *f);
    if (!f) {
        CloseHandle(h);
        return UMI_STATUS_OUT_OF_MEMORY;
    }
    f->handle=h;
    f->bytes=expected->bytes;
    f->writable=write;
    int system=0;
    if (write)s=Volumes(number,f,&system);
    if (s==UMI_STATUS_OK) {
        memset(&observed,0,sizeof observed);
        strcpy(observed.path,expected->path);
        s=Observe(h,number,&observed);
    }
    if (s==UMI_STATUS_OK&&!BmSameDevice(expected,&observed))s=UMI_STATUS_INVALID_STATE;
    DWORD got=0;
    if (s==UMI_STATUS_OK&&write&&!DeviceIoControl(h,IOCTL_DISK_IS_WRITABLE,NULL,0,NULL,0,&got,NULL))s=UMI_STATUS_PERMISSION_DENIED;
    if (s!=UMI_STATUS_OK) {
        BmClose(f);
        return s;
    }
    *out=f;
    return UMI_STATUS_OK;
}
