#include "test_kalman.h"
#include "common.h"
/**
 * 变量定义：
 * Xout为最终预测值
 * X为k时刻的预测值
 * x为k-1时刻的预测值
 * Pout为最终协方差矩阵
 * P为k时刻的协方差矩阵
 * p为k-1时刻的协方差矩阵
*/

double F_k[1][1] = {0};
typedef struct
{
    double *F;      /* 状态转换矩阵，根据实际运动规律的公式得到 */
    double *B;      /* 外部控制矩阵，对系统有影响，但却不会捕获到的变量的参数，根据实际运动规律的公式得到 */
    double *u;      /* 外部控制变量，对系统有影响，但却不会捕获到，例如加速度a */
    double *x;      /* 上一次的最优评估结果 */
    double *P;      /* 上一次的最优协方差矩阵 */
    // double *Q;      /* 噪声矩阵，根据当前评估的临时x计算得到的协方差矩阵 */
}kalman_param_t;

// static void matrix_multiplication(int line1, int column1, int column2, int arr1[MAX][MAX], int arr2[MAX][MAX], int arr3[MAX][MAX])
// {
//     int i, j, k;
//     int ret;
//     for (i = 0; i < line1; i++)
//     {
//         for (j = 0; j < column1; j++)
//         {
//             ret = 0;
//             for (k = 0; k < column2+1; k++)
//             {
//                 ret += arr1[i][k] * arr2[k][j];   //按照矩阵的乘法规则进行运算
//             }
//             arr3[i][j] = ret;
//         }
//     }
// }

int test_kalman_init(void)
{
    _INFO("%s\n", __FILE__);

    return 0;
}

void demo(void)
{
    /* 测量矩阵 */
    const double t = 0.3;

    /* F */
    /**
     * F=
     * {
     *  1, t
     *  0, 1
     * }
    */
    double F[2][2] = {
        {1, t}, 
        {0, 1}
    };
    double x[2] = {0};
}
