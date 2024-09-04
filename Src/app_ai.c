// #include <rtthread.h>
// #include "app_ai.h"
// #include <stdlib.h>
// #include <string.h>
// #include <math.h>
// #include "network.h"
// #include "network_data.h"
// #include "bsp_usart.h"

// #define DBG_SECTION_NAME "AI"
// #define DBG_LEVEL DBG_LOG
// #include <rtdbg.h>

// static ai_handle network = AI_HANDLE_NULL;
// static ai_network_report network_info;

// AI_ALIGNED(4)
// static ai_u8 activations[AI_NETWORK_DATA_ACTIVATIONS_SIZE];

// #if !defined(AI_NETWORK_INPUTS_IN_ACTIVATIONS)
// AI_ALIGNED(4)
// static ai_u8 in_data_s[AI_NETWORK_IN_1_SIZE_BYTES];
// #endif

// #if !defined(AI_NETWORK_OUTPUTS_IN_ACTIVATIONS)
// AI_ALIGNED(4)
// static ai_u8 out_data_s[AI_NETWORK_OUT_1_SIZE_BYTES];
// #endif

// static void ai_log_err(const ai_error err, const char *fct)
// {
//     if (fct)
//         printf("TEMPLATE - Error (%s) - type=0x%02x code=0x%02x\r\n", fct, err.type, err.code);
//     else
//         printf("TEMPLATE - Error - type=0x%02x code=0x%02x\r\n", err.type, err.code);
// }

// static int ai_boostrap(ai_handle w_addr, ai_handle act_addr)
// {
//     ai_error err;

//     /* 1 - Create an instance of the model */
//     err = ai_network_create(&network, AI_NETWORK_DATA_CONFIG);
//     if (err.type != AI_ERROR_NONE)
//     {
//         ai_log_err(err, "ai_network_create");
//         return -1;
//     }

//     /* 2 - Initialize the instance */
//     const ai_network_params params = AI_NETWORK_PARAMS_INIT(
//         AI_NETWORK_DATA_WEIGHTS(w_addr),
//         AI_NETWORK_DATA_ACTIVATIONS(act_addr));

//     if (!ai_network_init(network, &params))
//     {
//         err = ai_network_get_error(network);
//         ai_log_err(err, "ai_network_init");
//         return -1;
//     }

//     /* 3 - Retrieve the network info of the created instance */
//     if (!ai_network_get_info(network, &network_info))
//     {
//         err = ai_network_get_error(network);
//         ai_log_err(err, "ai_network_get_error");
//         ai_network_destroy(network);
//         network = AI_HANDLE_NULL;
//         return -3;
//     }

//     return 0;
// }

// static int ai_run(void *data_in, void *data_out)
// {
//     ai_i32 batch;

//     ai_buffer *ai_input = network_info.inputs;
//     ai_buffer *ai_output = network_info.outputs;

//     ai_input[0].data = AI_HANDLE_PTR(data_in);
//     ai_output[0].data = AI_HANDLE_PTR(data_out);

//     batch = ai_network_run(network, ai_input, ai_output);
//     if (batch != 1)
//     {
//         ai_log_err(ai_network_get_error(network), "ai_network_run");
//         return -1;
//     }

//     return 0;
// }

// void MX_X_CUBE_AI_Init(void)
// {
//     printf("\r\nTEMPLATE - initialization\r\n");

//     CRC_HandleTypeDef hcrc;
//     hcrc.Instance = CRC;
//     HAL_CRC_Init(&hcrc);

//     ai_boostrap(ai_network_data_weights_get(), activations);
// }
// // INIT_APP_EXPORT(MX_X_CUBE_AI_Init);

// /* 自定义网络查询及配置命令行接口 */
// static void ai_test(int argc, char *argv[])
// {
//     if (argc != 2)
//     {
//         printf("use ai <float>\r\n");
//         return;
//     }

//     float test_data = atof(argv[1]);

//     if (network == AI_HANDLE_NULL)
//     {
//         ai_error err = {AI_ERROR_INVALID_HANDLE, AI_ERROR_CODE_NETWORK};
//         ai_log_err(err, "network not init ok");
//         return;
//     }

//     if ((network_info.n_inputs != 1) || (network_info.n_outputs != 1))
//     {
//         ai_error err = {AI_ERROR_INVALID_PARAM, AI_ERROR_CODE_OUT_OF_RANGE};
//         ai_log_err(err, "template code should be updated\r\n to support a model with multiple IO");
//         return;
//     }

//     // int ret = ai_run(&test_data, out_data_s);
//     // printf("input data %.2f\r\n", test_data);
//     // printf("output data [%.2f  %.2f  %.2f]\r\n", *((float *)&out_data_s[0]),
//     //        *((float *)&out_data_s[4]),
//     //        *((float *)&out_data_s[8]));
//     // printf("output data [%d  %d  %d]\r\n", (uint8_t)round(*((float *)&out_data_s[0])),
//     //        (uint8_t)round(*((float *)&out_data_s[4])),
//     //        (uint8_t)round(*((float *)&out_data_s[8])));
// }

// // MSH_CMD_EXPORT_ALIAS(ai_test, ai, ai_test sample);

#include "app_ai.h"
// #include "bsp_usart.h"
// #include "stdio.h"

// ai_handle network;

// float aiInData[AI_NETWORK_IN_1_SIZE];
// float aiOutData[AI_NETWORK_OUT_1_SIZE];
// ai_u8 activations[AI_NETWORK_DATA_ACTIVATIONS_SIZE];
// const char *activities[AI_NETWORK_OUT_1_SIZE] = {
//     "low", "mid", "high"};

// ai_buffer *ai_input;
// ai_buffer *ai_output;

// void AI_Init(void)
// {
//     ai_error err;

//     /* Create a local array with the addresses of the activations buffers */
//     const ai_handle act_addr[] = {activations};
//     /* Create an instance of the model */
//     err = ai_network_create_and_init(&network, act_addr, NULL);
//     if (err.type != AI_ERROR_NONE)
//     {
//         printf("ai_network_create error - type=%d code=%d\r\n", err.type, err.code);
//         Error_Handler();
//     }
//     ai_input = ai_network_inputs_get(network, NULL);
//     ai_output = ai_network_outputs_get(network, NULL);
// }

// void AI_Run(float *pIn, float *pOut)
// {
//     ai_i32 batch;
//     ai_error err;

//     /* Update IO handlers with the data payload */
//     ai_input[0].data = AI_HANDLE_PTR(pIn);
//     ai_output[0].data = AI_HANDLE_PTR(pOut);

//     batch = ai_network_run(network, ai_input, ai_output);
//     if (batch != 1)
//     {
//         err = ai_network_get_error(network);
//         printf("AI ai_network_run error - type=%d code=%d\r\n", err.type, err.code);
//         Error_Handler();
//     }
// }

// uint32_t argmax(const float *values, uint32_t len)
// {
//     float max_value = values[0];
//     uint32_t max_index = 0;
//     for (uint32_t i = 1; i < len; i++)
//     {
//         if (values[i] > max_value)
//         {
//             max_value = values[i];
//             max_index = i;
//         }
//     }
//     return max_index;
// }