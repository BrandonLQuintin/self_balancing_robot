#include "can_communication.h"
#include "main.h"

static const CAN_TxHeaderTypeDef tx_header = {
  .IDE = CAN_ID_STD,
  .StdId = 0x446U,
  .RTR = CAN_RTR_DATA,
  .DLC = 2U
};

static const uint8_t tx_data[8] = {50U, 0xAAU, 0U, 0U, 0U, 0U, 0U, 0U};
static uint32_t tx_mailbox = 0U;

void can_test_transmission(CAN_HandleTypeDef *hcan) {
  if (HAL_CAN_AddTxMessage(hcan, &tx_header, tx_data, &tx_mailbox) != HAL_OK) {
    Error_Handler();
  }
}
