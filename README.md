# AT24CxEeprom
Arduino library for AT24C type eeproms

Supports Chips from 1Kbit (128 Bytes) to 2Mbit (262144 bytes): AT24C01, AT24C02, AT24C04, AT24C08, AT24C16, AT24C32, AT24C64, AT24C128, AT24C256, AT24C512, AT24CM01, AT24CM02
Write Cycle Time of the chip is taken care of by waiting 1ms after having received a NACK (not acknowledge) and doing a retry of up to 10 times.
