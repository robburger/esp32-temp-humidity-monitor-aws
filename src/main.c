/*
 * Copyright (c) 2014-2019 Cesanta Software Limited
 * All rights reserved
 *
 * Licensed under the Apache License, Version 2.0 (the ""License"");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an ""AS IS"" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "mgos.h"
#include "mgos_dht.h"
#include "mgos_rpc.h"

static struct mgos_dht *s_dht = NULL;

static void dht_timer_cb(void *arg) {
  float t = mgos_dht_get_temp(s_dht);
  float h = mgos_dht_get_humidity(s_dht);

  if (isnan(h) || isnan(t)) {
    LOG(LL_INFO, ("Failed to read data from sensor"));
    return;
  }
  LOG(LL_INFO, ("Temperature: %.2f *C Humidity: %.2f %%", t, h));
  (void) arg;
}

static void rpc_dht_temp_cb(struct mg_rpc_request_info *ri,
                            void *cb_arg,
                            struct mg_rpc_frame_info *fi,
                            struct mg_str args) {
  mg_rpc_send_responsef(ri, "{value: %.2f}", mgos_dht_get_temp(cb_arg));
  (void) fi;
  (void) args;
}

static void rpc_dht_humidity_cb(struct mg_rpc_request_info *ri,
                            void *cb_arg,
                            struct mg_rpc_frame_info *fi,
                            struct mg_str args) {
  mg_rpc_send_responsef(ri, "{value: %.2f}", mgos_dht_get_humidity(cb_arg));
  (void) fi;
  (void) args;
}

enum mgos_app_init_result mgos_app_init(void) {
  if ((s_dht = mgos_dht_create(mgos_sys_config_get_dht_pin(), DHT22)) == NULL)
    return MGOS_APP_INIT_ERROR;
  mgos_set_timer((mgos_sys_config_get_dht_freq() * 1000), true, dht_timer_cb, NULL);
  mg_rpc_add_handler(mgos_rpc_get_global(), "Temp.Read", "", rpc_dht_temp_cb, s_dht);
  mg_rpc_add_handler(mgos_rpc_get_global(), "Humidity.Read", "", rpc_dht_humidity_cb, s_dht);
  return MGOS_APP_INIT_SUCCESS;
}
