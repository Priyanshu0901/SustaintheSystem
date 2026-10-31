#include "board_identity.h"
#include "esp_mac.h"
#include <stdint.h>
#include <stdio.h>

bool board_identity_read(char output[18])
{
    /* esp_efuse_mac_get_default writes eight bytes on C6, not six. Request
     * the base interface explicitly and reserve the SDK maximum regardless. */
    uint8_t mac[8]={0};
    output[0]=0;
    if (esp_read_mac(mac,ESP_MAC_BASE)!=ESP_OK) return false;
    snprintf(output,18,"%02X:%02X:%02X:%02X:%02X:%02X",mac[0],mac[1],mac[2],mac[3],mac[4],mac[5]);
    return true;
}
