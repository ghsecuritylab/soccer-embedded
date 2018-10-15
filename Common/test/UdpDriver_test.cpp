/**
  *****************************************************************************
  * @file    UdpDriver_test.cpp
  * @author  Robert Fairley
  * @brief   Unit tests for UDP driver.
  *
  * @defgroup udp_driver_test
  * @brief    Unit tests for the UdpDriver class.
  * @{
  *****************************************************************************
  */

#include <MockOsInterface.h>
#include <MockUdpInterface.h>
#include <UdpDriver.h>

#include <gtest/gtest.h>
#include <gmock/gmock.h>

// TODO: should move the consts to defines. Only works as const because they are literal type.
constexpr u16_t ZERO_U16_T = 0;
const ip_addr_t ZERO_IP_ADDR_T = {0x0};
const ip_addr_t TEST_IP_ADDR = {0xC0A80008};
const ip_addr_t TEST_IP_ADDR_PC = {0xC0A80002};
constexpr u16_t TEST_PORT = 7;
constexpr u16_t TEST_PORT_PC = 6340;
const udp_interface::UdpInterface *NON_NULL_PTR_UDP_INTERFACE =
        (udp_interface::UdpInterface *) 0x71;
const os::OsInterface *NON_NULL_PTR_FREERTOS_INTERFACE =
        (os::OsInterface *) 0x72;
struct udp_pcb *NON_NULL_PTR_PCB = (struct udp_pcb *) 0x51;
struct pbuf_t *NON_NULL_PTR_PBUF = (struct pbuf_t *) 0x61;

using ::testing::Return;
using ::testing::_;

using namespace udp_driver;

bool operator==(const ip_addr_t& lhs, const ip_addr_t& rhs) {
	return lhs.addr == rhs.addr;
}

TEST(UdpDriverTests, IpAddrDefaultInitializesToZeroIpAddrT) {
    udp_driver::UdpDriver udpDriverUnderTest; // TODO: should modify constructor to pass in "MOCK" or "REAL" rather than pass in a the interface
    ASSERT_EQ(udpDriverUnderTest.getIpaddr(), ZERO_IP_ADDR_T);
}

TEST(UdpDriverTests, IpAddrPcDefaultInitializesToZeroIpAddrT) {
    udp_driver::UdpDriver udpDriverUnderTest;
    ASSERT_EQ(udpDriverUnderTest.getIpaddrPc(), ZERO_IP_ADDR_T);
}

TEST(UdpDriverTests, PortDefaultInitializesToZero) {
    udp_driver::UdpDriver udpDriverUnderTest;
    ASSERT_EQ(udpDriverUnderTest.getPort(), ZERO_U16_T);
}

TEST(UdpDriverTests, PortPcDefaultInitializesToZero) {
    udp_driver::UdpDriver udpDriverUnderTest;
    ASSERT_EQ(udpDriverUnderTest.getPortPc(), ZERO_U16_T);
}

TEST(UdpDriverTests, UdpInterfaceDefaultInitializesToNull) {
    udp_driver::UdpDriver udpDriverUnderTest;
    ASSERT_EQ(udpDriverUnderTest.getUdpInterface(), nullptr);
}

TEST(UdpDriverTests, OsInterfaceDefaultInitializesToNull) {
    udp_driver::UdpDriver udpDriverUnderTest;
    ASSERT_EQ(udpDriverUnderTest.getOsInterface(), nullptr);
}

TEST(UdpDriverTests, PcbDefaultInitializesToNull) {
    udp_driver::UdpDriver udpDriverUnderTest;
    ASSERT_EQ(udpDriverUnderTest.getPcb(), nullptr);
}

TEST(UdpDriverTests, RxPbufDefaultInitializesToNull) {
    udp_driver::UdpDriver udpDriverUnderTest;
    ASSERT_EQ(udpDriverUnderTest.getRxPbuf(), nullptr);
}

TEST(UdpDriverTests, TxPbufDefaultInitializesToNull) {
    udp_driver::UdpDriver udpDriverUnderTest;
    ASSERT_EQ(udpDriverUnderTest.getTxPbuf(), nullptr);
}

TEST(UdpDriverTests, RxPbufDefaultInitializesToNullThreaded) {
    mocks::MockOsInterface mockOsInterface;
    udp_driver::UdpDriver udpDriverUnderTest(ZERO_IP_ADDR_T, ZERO_IP_ADDR_T, ZERO_U16_T,
            ZERO_U16_T, nullptr, &mockOsInterface);
    ASSERT_EQ(udpDriverUnderTest.getRxPbufThreaded(), nullptr);
}

TEST(UdpDriverTests, TxPbufDefaultInitializesToNullThreaded) {
    mocks::MockOsInterface mockOsInterface;
    udp_driver::UdpDriver udpDriverUnderTest(ZERO_IP_ADDR_T, ZERO_IP_ADDR_T, ZERO_U16_T,
            ZERO_U16_T, nullptr, &mockOsInterface);
    ASSERT_EQ(udpDriverUnderTest.getTxPbufThreaded(), nullptr);
}

TEST(UdpDriverTests, CanInitializeIpaddrWithParameterizedConstructor) {
    udp_driver::UdpDriver udpDriverUnderTest(TEST_IP_ADDR, ZERO_IP_ADDR_T, ZERO_U16_T,
            ZERO_U16_T, nullptr, nullptr);
    ASSERT_EQ(TEST_IP_ADDR, udpDriverUnderTest.getIpaddr());
}

TEST(UdpDriverTests, CanInitializeIpaddrPcWithParameterizedConstructor) {
    udp_driver::UdpDriver udpDriverUnderTest(ZERO_IP_ADDR_T, TEST_IP_ADDR_PC,
            ZERO_U16_T, ZERO_U16_T, nullptr, nullptr);
    ASSERT_EQ(TEST_IP_ADDR_PC, udpDriverUnderTest.getIpaddrPc());
}

TEST(UdpDriverTests, CanInitializePortWithParameterizedConstructor) {
    udp_driver::UdpDriver udpDriverUnderTest(ZERO_IP_ADDR_T, ZERO_IP_ADDR_T, TEST_PORT,
            ZERO_U16_T, nullptr, nullptr);
    ASSERT_EQ(TEST_PORT, udpDriverUnderTest.getPort());
}

TEST(UdpDriverTests, CanInitializePortPcWithParameterizedConstructor) {
    udp_driver::UdpDriver udpDriverUnderTest(ZERO_IP_ADDR_T, ZERO_IP_ADDR_T, ZERO_U16_T,
            TEST_PORT_PC, nullptr, nullptr);
    ASSERT_EQ(TEST_PORT_PC, udpDriverUnderTest.getPortPc());
}

TEST(UdpDriverTests, CanInitializeUdpInterfaceWithParameterizedConstructor) {
    mocks::MockUdpInterface mockUdpInterface;
    udp_driver::UdpDriver udpDriverUnderTest(ZERO_IP_ADDR_T, ZERO_IP_ADDR_T, ZERO_U16_T,
            ZERO_U16_T, &mockUdpInterface, nullptr);
    ASSERT_EQ(udpDriverUnderTest.getUdpInterface(), &mockUdpInterface);
}

TEST(UdpDriverTests, CanInitializeOsInterfaceWithParameterizedConstructor) {
    mocks::MockOsInterface mockOsInterface;
    udp_driver::UdpDriver udpDriverUnderTest(ZERO_IP_ADDR_T, ZERO_IP_ADDR_T, ZERO_U16_T,
            ZERO_U16_T, nullptr, &mockOsInterface);
    ASSERT_EQ(udpDriverUnderTest.getOsInterface(), &mockOsInterface);
}

TEST(UdpDriverTests, FunctionCallsCorrectOrderSetupSuccess) {
    mocks::MockUdpInterface mockUdpInterface;
    mocks::MockOsInterface mockOsInterface;
    EXPECT_CALL(mockUdpInterface, udpRecv(_, _, _)).Times(1);
    EXPECT_CALL(mockUdpInterface, udpBind(_, _, _)).Times(1).WillOnce(
            Return(ERR_OK));
    EXPECT_CALL(mockUdpInterface, udpNew()).Times(1).WillOnce(
            Return(NON_NULL_PTR_PCB));

    UdpDriver udpDriverUnderTest(ZERO_IP_ADDR_T, ZERO_IP_ADDR_T, ZERO_U16_T, ZERO_U16_T,
            &mockUdpInterface, &mockOsInterface);
    bool success = udpDriverUnderTest.setup();
    ASSERT_TRUE(success);
}

TEST(UdpDriverTests, FunctionCallsCorrectOrderSetupFailsOnNew) {
    mocks::MockUdpInterface mockUdpInterface;
    mocks::MockOsInterface mockOsInterface;
    EXPECT_CALL(mockUdpInterface, udpNew()).Times(1).WillOnce(Return(nullptr));

    UdpDriver udpDriverUnderTest(ZERO_IP_ADDR_T, ZERO_IP_ADDR_T, ZERO_U16_T, ZERO_U16_T,
            &mockUdpInterface, &mockOsInterface);
    bool success = udpDriverUnderTest.setup();
    ASSERT_FALSE(success);
}

TEST(UdpDriverTests, FunctionCallsCorrectOrderSetupFailsOnBind) {
    mocks::MockUdpInterface mockUdpInterface;
    mocks::MockOsInterface mockOsInterface;
    EXPECT_CALL(mockUdpInterface, udpRemove(_)).Times(1); // Assume removal happens without error
    EXPECT_CALL(mockUdpInterface, udpBind(_, _, _)).Times(1).WillOnce(
            Return(ERR_USE));
    EXPECT_CALL(mockUdpInterface, udpNew()).Times(1).WillOnce(
            Return(NON_NULL_PTR_PCB));

    UdpDriver udpDriverUnderTest(ZERO_IP_ADDR_T, ZERO_IP_ADDR_T, ZERO_U16_T, ZERO_U16_T,
            &mockUdpInterface, &mockOsInterface);
    bool success = udpDriverUnderTest.setup();
    ASSERT_FALSE(success);
}

TEST(UdpDriverTests, FunctionCallsCorrectOrderReceiveSuccess) {
    mocks::MockUdpInterface mockUdpInterface;
    mocks::MockOsInterface mockOsInterface;
    EXPECT_CALL(mockUdpInterface, pbufFree(_)).Times(1).WillOnce(
            Return((u8_t) 1));
    EXPECT_CALL(mockOsInterface, OS_xSemaphoreTake(_, _)).Times(1).WillOnce(
            Return(pdTRUE));

    UdpDriver udpDriverUnderTest(ZERO_IP_ADDR_T, ZERO_IP_ADDR_T, ZERO_U16_T, ZERO_U16_T,
            &mockUdpInterface, &mockOsInterface);
    bool success = udpDriverUnderTest.receive(nullptr);
    ASSERT_TRUE(success);
}

TEST(UdpDriverTests, FunctionCallsCorrectOrderReceiveFailsOnSemaphoreTake) {
    mocks::MockUdpInterface mockUdpInterface;
    mocks::MockOsInterface mockOsInterface;
    EXPECT_CALL(mockOsInterface, OS_xSemaphoreTake(_, _)).Times(1).WillOnce(
            Return(pdFALSE));

    UdpDriver udpDriverUnderTest(ZERO_IP_ADDR_T, ZERO_IP_ADDR_T, ZERO_U16_T, ZERO_U16_T,
            &mockUdpInterface, &mockOsInterface);
    bool success = udpDriverUnderTest.receive(nullptr);
    ASSERT_FALSE(success);
}

TEST(UdpDriverTests, FunctionCallsCorrectOrderReceiveFailsOnPbufFreeNoneFreed) {
    mocks::MockUdpInterface mockUdpInterface;
    mocks::MockOsInterface mockOsInterface;
    EXPECT_CALL(mockUdpInterface, pbufFree(_)).Times(1).WillOnce(
            Return((u8_t) 0));
    EXPECT_CALL(mockOsInterface, OS_xSemaphoreTake(_, _)).Times(1).WillOnce(
            Return(pdTRUE));

    UdpDriver udpDriverUnderTest(ZERO_IP_ADDR_T, ZERO_IP_ADDR_T, ZERO_U16_T, ZERO_U16_T,
            &mockUdpInterface, &mockOsInterface);
    bool success = udpDriverUnderTest.receive(nullptr);
    ASSERT_FALSE(success);
}

TEST(UdpDriverTests, FunctionCallsCorrectOrderTransmitSuccess) {
    mocks::MockUdpInterface mockUdpInterface;
    EXPECT_CALL(mockUdpInterface, pbufFree(_)).Times(1).WillOnce(
            Return((u8_t) 1));
    EXPECT_CALL(mockUdpInterface, udpDisconnect(_)).Times(1);
    EXPECT_CALL(mockUdpInterface, udpSend(_, _)).Times(1).WillOnce(
            Return(ERR_OK));
    EXPECT_CALL(mockUdpInterface, udpConnect(_, _, _)).Times(1).WillOnce(
            Return(ERR_OK));

    UdpDriver udpDriverUnderTest(ZERO_IP_ADDR_T, ZERO_IP_ADDR_T, ZERO_U16_T, ZERO_U16_T,
            &mockUdpInterface, nullptr);
    bool success = udpDriverUnderTest.transmit(nullptr);
    ASSERT_TRUE(success);
}

TEST(UdpDriverTests, FunctionCallsCorrectOrderTransmitFailsOnConnect) {
    mocks::MockUdpInterface mockUdpInterface;
    EXPECT_CALL(mockUdpInterface, udpConnect(_, _, _)).Times(1).WillOnce(
            Return(ERR_VAL)); // use ERR_VAL; anything but ERR_OK works

    UdpDriver udpDriverUnderTest(ZERO_IP_ADDR_T, ZERO_IP_ADDR_T, ZERO_U16_T, ZERO_U16_T,
            &mockUdpInterface, nullptr);
    bool success = udpDriverUnderTest.transmit(nullptr);
    ASSERT_FALSE(success);
}

TEST(UdpDriverTests, FunctionCallsCorrectOrderTransmitFailsOnSend) {
    mocks::MockUdpInterface mockUdpInterface;
    EXPECT_CALL(mockUdpInterface, udpSend(_, _)).Times(1).WillOnce(
            Return(ERR_VAL));
    EXPECT_CALL(mockUdpInterface, udpConnect(_, _, _)).Times(1).WillOnce(
            Return(ERR_OK));

    UdpDriver udpDriverUnderTest(ZERO_IP_ADDR_T, ZERO_IP_ADDR_T, ZERO_U16_T, ZERO_U16_T,
            &mockUdpInterface, nullptr);
    bool success = udpDriverUnderTest.transmit(nullptr);
    ASSERT_FALSE(success);
}

TEST(UdpDriverTests, FunctionCallsCorrectOrderTransmitFailsOnPbufFreeNoneFreed) {
    mocks::MockUdpInterface mockUdpInterface;
    EXPECT_CALL(mockUdpInterface, pbufFree(_)).Times(1).WillOnce(
            Return((u8_t) 0));
    EXPECT_CALL(mockUdpInterface, udpDisconnect(_)).Times(1);
    EXPECT_CALL(mockUdpInterface, udpSend(_, _)).Times(1).WillOnce(
            Return(ERR_OK));
    EXPECT_CALL(mockUdpInterface, udpConnect(_, _, _)).Times(1).WillOnce(
            Return(ERR_OK));

    UdpDriver udpDriverUnderTest(ZERO_IP_ADDR_T, ZERO_IP_ADDR_T, ZERO_U16_T, ZERO_U16_T,
            &mockUdpInterface, nullptr);
    bool success = udpDriverUnderTest.transmit(nullptr);
    ASSERT_FALSE(success);
}

// TODO: tests that vary the arguments passed in. test for non null arguments?