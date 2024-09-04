/**
  ******************************************************************************
  * @file    network_data_params.c
  * @author  AST Embedded Analytics Research Platform
  * @date    Tue Jul 23 17:41:13 2024
  * @brief   AI Tool Automatic Code Generator for Embedded NN computing
  ******************************************************************************
  * Copyright (c) 2024 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  ******************************************************************************
  */

#include "network_data_params.h"


/**  Activations Section  ****************************************************/
ai_handle g_network_activations_table[1 + 2] = {
  AI_HANDLE_PTR(AI_MAGIC_MARKER),
  AI_HANDLE_PTR(NULL),
  AI_HANDLE_PTR(AI_MAGIC_MARKER),
};




/**  Weights Section  ********************************************************/
AI_ALIGNED(32)
const ai_u64 s_network_weights_array_u64[98] = {
  0xbed22d403db2483aU, 0x3dc2fb103bca0e00U, 0xbf4b69e23d83673aU, 0xbf660a1cbf5d915cU,
  0xbdb4dd70bedbd478U, 0x3ed0cce7bf5c7008U, 0xbc684e39be9b3508U, 0x3f2332c7bf362ce8U,
  0xbf6acaa83e9a459dU, 0x3f0f53b7bf1f12e6U, 0xbf49d1d03f666f5dU, 0xbf52ebb4bf0c3856U,
  0xbeb53cec3f245cbaU, 0xbf6279c23eef0ea8U, 0x3e8be1813ecb4abcU, 0xbf4a0f16bf1f89beU,
  0xbd4a5a203e534fe6U, 0xbe5f99d6be09aa2eU, 0x3dcde0f4be50addaU, 0x3dab630cbd2757f0U,
  0x3e18428a3e67e8feU, 0x3d2ef6503e2afc6eU, 0xbdc3c2883e21ca24U, 0xbdb420b8bd61f490U,
  0x3ca833f03dbc3dbbU, 0x3dcddb213db5433cU, 0xbdae670cbd99da47U, 0xbcba98603e28ef66U,
  0xbe1b31343e4e4bbaU, 0x3cf806c7bae97e00U, 0xbc569af1bdde9d14U, 0x3e9b15fe3e0527a0U,
  0xbe23196cbda6a5adU, 0x3de2703abe0fdd12U, 0x3dd9f8dcbe531362U, 0xbe5aecc23d5f64c8U,
  0x3cce6f203e3f9a5aU, 0x3e24d37abe2d5826U, 0xbe18ff823e0d16d4U, 0x3b8ddefdbe02654eU,
  0x3e260d44bd40c7b3U, 0xbcf10dbabe3827d4U, 0x3d94353c3db0413fU, 0x3e307a603d97f2c4U,
  0xbd8a01943e06ce90U, 0x3cf72a883e2c87c6U, 0x3e88c16abe6576dcU, 0x3daa6996bdc00e24U,
  0xbe7e46f4be02dc45U, 0x3dbd95f73bb7f0c0U, 0xbd81502cbd628ee1U, 0xbd52d330be6eadf2U,
  0x3e4743d83d955508U, 0xbd9302f3be591c88U, 0xbe074c4dbe18cf2cU, 0x3e496233be4c2c8aU,
  0x3d2fdff8be2d2666U, 0x3c7de090bda9fb38U, 0x3e2f3d42bd84883bU, 0xbde6d88cbe1c6a0aU,
  0xbd20b4403e5b60baU, 0x3da8da11bd4c94a0U, 0xbe368e56bd4573c8U, 0x3e1bf1073e4f3cccU,
  0xbe3720a4bd3f35a0U, 0x3ddff2f83e1480d8U, 0x3e50a90c3d314d30U, 0x3da7b23cbcad2420U,
  0x3e02d0123e2279aeU, 0x3cdbaf103d4048f8U, 0x3e6f554ebe180114U, 0xbde3fa78bb6b1f80U,
  0x3e7f3f023e0622e8U, 0xbe52e156bd3d7598U, 0xbcaadb403e7bb75aU, 0xbe502456be7aca28U,
  0x3e7c43c6bd6917a8U, 0xbe3cf630be2ab8b2U, 0x3d5f4cd83e32d2d2U, 0xbd4ed6b03de5c2f4U,
  0xbdc7d1143e4e78f4U, 0x3cf9f036be03fd75U, 0xbe7ae4a03e51dd52U, 0xbd93888c3cf20150U,
  0xbe1e9c36bd85aee8U, 0x3e99f7acbe4ba07cU, 0x3e314a05be84b4b8U, 0xbe970b84be84d946U,
  0xbd102156bd25c918U, 0x3cbb1173be7b2bd1U, 0x3eaa3ef73eb6cfdcU, 0x3e89e3f73ea62c61U,
  0xbed14ff7bbba5580U, 0xbd8fa1ae3d67af26U, 0x3e7ac6a1bdaf3637U, 0x3e0ee4fabe316bb3U,
  0xbe4a6c893ead4b62U, 0xbec6317cU,
};


ai_handle g_network_weights_table[1 + 2] = {
  AI_HANDLE_PTR(AI_MAGIC_MARKER),
  AI_HANDLE_PTR(s_network_weights_array_u64),
  AI_HANDLE_PTR(AI_MAGIC_MARKER),
};

