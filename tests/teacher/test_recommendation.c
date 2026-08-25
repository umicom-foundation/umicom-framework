#include "umicom/teacher/recommendation.h"
int main(void) { UmiTeacherRecommendationScore a={80U,90U,75U,0U},b={20U,50U,50U,0U}; if(umi_teacher_recommendation_compute(&a)<70U) return 1; if(!umi_teacher_recommendation_prefer(&a,&b)) return 2; return 0; }
