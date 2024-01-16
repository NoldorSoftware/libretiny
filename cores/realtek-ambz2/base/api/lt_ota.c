/* Copyright (c) Kuba Szczodrzyński 2023-05-22. */

#include <libretiny.h>
#include <sdk_private.h>
#include <sys_api.h>

// extern uint32_t get_cur_fw_idx(void);
// extern uint32_t get_number_of_fw_valid(void);
extern void get_fw_info(uint32_t *targetFWaddr, uint32_t *currentFWaddr, uint32_t *fw1_sn, uint32_t *fw2_sn);
// extern uint32_t sys_update_ota_prepare_addr(void);

lt_ota_type_t lt_ota_get_type() {
	return OTA_TYPE_DUAL;
}

bool lt_ota_is_valid(uint8_t index) {
	// return false;
	uint32_t offset;
	switch (index) {
		case 1:
			offset = FLASH_OTA1_OFFSET;
			LT_IM(OTA, "lt_ota_is_valid is FLASH_OTA1_OFFSET");
			break;
		case 2:
			offset = FLASH_OTA2_OFFSET;
			LT_IM(OTA, "lt_ota_is_valid is FLASH_OTA2_OFFSET");
			break;
		default:
			LT_IM(OTA, "lt_ota_is_valid returning true");
			return false;
	}
	uint8_t *address = (uint8_t *)(SPI_FLASH_BASE + offset);
	bool val = memcmp(address, "81958711", 8) == 0;
	if (val)
	{
		LT_IM(OTA, "lt_ota_is_valid returning true val");
	}
	else {
		LT_IM(OTA, "lt_ota_is_valid returning false val");
	}
	return val;
}

uint8_t lt_ota_dual_get_current() {
	LT_IM(OTA, "lt_ota_dual_get_current()");

	uint32_t targetFWaddr;
	uint32_t currentFWaddr;
	uint32_t fw1_sn;
	uint32_t fw2_sn;
	get_fw_info(&targetFWaddr, &currentFWaddr, &fw1_sn, &fw2_sn);

	LT_IM(OTA, "targetFWaddr = %x", targetFWaddr);
	LT_IM(OTA, "currentFWaddr = %x", currentFWaddr);
	LT_IM(OTA, "fw1_sn = %x", fw1_sn);
	LT_IM(OTA, "fw2_sn = %x", fw2_sn);

	if (currentFWaddr == FLASH_OTA1_OFFSET)
	{
		return 1;
	}
	else if (currentFWaddr == FLASH_OTA2_OFFSET)
	{
		return 2;
	}

	return -1;
}

uint8_t lt_ota_dual_get_stored() {
	LT_IM(OTA, "lt_ota_dual_get_stored");

	uint32_t targetFWaddr;
	uint32_t currentFWaddr;
	uint32_t fw1_sn;
	uint32_t fw2_sn;
	get_fw_info(&targetFWaddr, &currentFWaddr, &fw1_sn, &fw2_sn);

	LT_IM(OTA, "targetFWaddr = %x", targetFWaddr);
	LT_IM(OTA, "currentFWaddr = %x", currentFWaddr);
	LT_IM(OTA, "fw1_sn = %x", fw1_sn);
	LT_IM(OTA, "fw2_sn = %x", fw2_sn);

	if (targetFWaddr == FLASH_OTA1_OFFSET)
	{
		return 1;
	}
	else if (targetFWaddr == FLASH_OTA2_OFFSET)
	{
		return 2;
	}

	return -1;
}

bool lt_ota_switch(bool revert) {
	LT_IM(OTA, "lt_ota_switch -> revert = %d", revert);
	
	uint8_t current = lt_ota_dual_get_current();
	uint8_t stored	= lt_ota_dual_get_stored();
	if ((current == stored) == revert)
		return true;

	if (!lt_ota_is_valid(stored ^ 0b11))
		return false;

	// - read current OTA switch value from 0x9004
	// - reset OTA switch to 0xFFFFFFFE if it's 0x0
	// - else check first non-zero bit of OTA switch
	// - write OTA switch with first non-zero bit cleared

	uint32_t value = HAL_READ32(SPI_FLASH_BASE, FLASH_SYSTEM_OFFSET + 4);
	if (value == 0) {
		uint8_t *system = (uint8_t *)malloc(64);
		lt_flash_read(FLASH_SYSTEM_OFFSET, system, 64);
		// reset OTA switch
		((uint32_t *)system)[1] = -2;
		lt_flash_erase_block(FLASH_SYSTEM_OFFSET);
		return lt_flash_write(FLASH_SYSTEM_OFFSET, system, 64);
	}

	// clear first non-zero bit
	value <<= 1;
	// write OTA switch to flash
	flash_write_word(NULL, FLASH_SYSTEM_OFFSET + 4, value);
	return true;

}
