/**
  *****************************************************************************
  * @file
  * @author  Robert Fairley
  *
  * @defgroup UdpDriver_Test
  * @ingroup  UDP
  * @brief    Unit tests for the UdpDriver class.
  * @{
  *****************************************************************************
  */

/* TODO: investigate threaded tests. */



/********************************* Includes **********************************/
#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "cmsis_os.h"
#include "OsInterfaceMock.h"
#include "UdpRawInterfaceMock.h"
#include "UdpDriver/UdpDriver.h"

using udp::UdpDriver;
using lwip::gmock::UdpRawInterfaceMock;
using cmsis::gmock::OsInterfaceMock;

using ::testing::Return;
using ::testing::_;
using ::testing::AnyNumber;




/* NOTE: needs to be defined outside of anonymous
namespace so gtest can use it. */
bool operator==(const ip_addr_t& lhs, const ip_addr_t& rhs) {
    return lhs.addr == rhs.addr;
}




/******************************** File-local *********************************/
namespace {
// Constants
// ----------------------------------------------------------------------------
const u32_t ZERO_IP_ADDR = 0x0;
const osThreadId TEST_THREAD_ID = reinterpret_cast<osThreadId>(5);
constexpr uint32_t TEST_SIGNAL = 0x00000050;

// Classes & structs
// ----------------------------------------------------------------------------
class UdpDriverTest : public ::testing::Test {
protected:
    UdpRawInterfaceMock udp_if;
    OsInterfaceMock os_if;
};

} // end anonymous namespace




TEST_F(UdpDriverTest, DefaultInitializeMembersToZero) {
    UdpDriver udpDriverUnderTest;

    EXPECT_EQ(udpDriverUnderTest.getIpAddrSrcVal(), ZERO_IP_ADDR);
    EXPECT_EQ(udpDriverUnderTest.getIpAddrDestVal(), ZERO_IP_ADDR);
    EXPECT_EQ(udpDriverUnderTest.getPortSrc(), (u16_t) 0);
    EXPECT_EQ(udpDriverUnderTest.getPortDest(), (u16_t) 0);
    EXPECT_EQ(udpDriverUnderTest.getUdpIf(), nullptr);
    EXPECT_EQ(udpDriverUnderTest.getOsIf(), nullptr);
}

TEST_F(UdpDriverTest, InitializeMembersWithParameterizedConstructor) {
    const u32_t TEST_IP_ADDR = 0xC0A80008;
    const u32_t TEST_IP_ADDR_PC = 0xC0A80002;

    UdpDriver udpDriverUnderTest(TEST_IP_ADDR, TEST_IP_ADDR_PC, (u16_t) 7,
            (u16_t) 6340, &udp_if, &os_if, TEST_SIGNAL, TEST_THREAD_ID);

    EXPECT_EQ(TEST_IP_ADDR, udpDriverUnderTest.getIpAddrSrcVal());
    EXPECT_EQ(TEST_IP_ADDR_PC, udpDriverUnderTest.getIpAddrDestVal());
    EXPECT_EQ((u16_t) 7, udpDriverUnderTest.getPortSrc());
    EXPECT_EQ((u16_t) 6340, udpDriverUnderTest.getPortDest());
    EXPECT_EQ(&udp_if, udpDriverUnderTest.getUdpIf());
    EXPECT_EQ(&os_if, udpDriverUnderTest.getOsIf());
}

TEST_F(UdpDriverTest, SucceedInitialize) {
    const u32_t TEST_IP_ADDR = 0xC0A80008;
    const u32_t TEST_IP_ADDR_PC = 0xC0A80002;

    EXPECT_CALL(os_if, OS_osMutexCreate(_)).Times(1).WillOnce(Return((osMutexId) 1));

    UdpDriver udpDriverUnderTest(TEST_IP_ADDR, TEST_IP_ADDR_PC, (u16_t) 7,
            (u16_t) 6340, &udp_if, &os_if, TEST_SIGNAL, TEST_THREAD_ID);

    ASSERT_TRUE(udpDriverUnderTest.initialize());
}

TEST_F(UdpDriverTest, FailInitializeWhenUdpInterfaceNull) {
    const u32_t TEST_IP_ADDR = 0xC0A80008;
    const u32_t TEST_IP_ADDR_PC = 0xC0A80002;

    UdpDriver udpDriverUnderTest(TEST_IP_ADDR, TEST_IP_ADDR_PC, (u16_t) 7,
            (u16_t) 6340, nullptr, &os_if, TEST_SIGNAL, TEST_THREAD_ID);

    ASSERT_FALSE(udpDriverUnderTest.initialize());
}

TEST_F(UdpDriverTest, FailInitializeWhenOsInterfaceNull) {
    const u32_t TEST_IP_ADDR = 0xC0A80008;
    const u32_t TEST_IP_ADDR_PC = 0xC0A80002;

    UdpDriver udpDriverUnderTest(TEST_IP_ADDR, TEST_IP_ADDR_PC, (u16_t) 7,
            (u16_t) 6340, &udp_if, nullptr, TEST_SIGNAL, TEST_THREAD_ID);

    ASSERT_FALSE(udpDriverUnderTest.initialize());
}

TEST_F(UdpDriverTest, FailInitializeWhenMutexCreateUnsuccessful) {
    const u32_t TEST_IP_ADDR = 0xC0A80008;
    const u32_t TEST_IP_ADDR_PC = 0xC0A80002;

    EXPECT_CALL(os_if, OS_osMutexCreate(_)).Times(1).WillOnce(Return((osMutexId) 0));

    UdpDriver udpDriverUnderTest(TEST_IP_ADDR, TEST_IP_ADDR_PC, (u16_t) 7,
            (u16_t) 6340, &udp_if, &os_if, TEST_SIGNAL, TEST_THREAD_ID);

    ASSERT_FALSE(udpDriverUnderTest.initialize());
}

TEST_F(UdpDriverTest, SucceedSetupReceive) {
    struct udp_pcb udpPcb;

    EXPECT_CALL(udp_if, udpRecv(_, _, _)).Times(1);
    EXPECT_CALL(udp_if, udpBind(_, _, _)).Times(1).WillOnce(Return(ERR_OK));
    EXPECT_CALL(os_if, OS_osMutexCreate(_)).Times(1).WillOnce(Return((osMutexId) 1));
    EXPECT_CALL(udp_if, udpNew()).Times(1).WillOnce(Return(&udpPcb));

    UdpDriver udpDriverUnderTest(ZERO_IP_ADDR, ZERO_IP_ADDR, (u16_t) 0, (u16_t) 0,
            &udp_if, &os_if, TEST_SIGNAL, TEST_THREAD_ID);
    ASSERT_TRUE(udpDriverUnderTest.initialize());
    ASSERT_TRUE(udpDriverUnderTest.setupReceive(nullptr));
    EXPECT_EQ(udpDriverUnderTest.getPcb(), &udpPcb);
}

TEST_F(UdpDriverTest, FailSetupReceiveOnUdpNew) {
    EXPECT_CALL(udp_if, udpNew()).Times(1).WillOnce(Return(nullptr));
    EXPECT_CALL(os_if, OS_osMutexCreate(_)).Times(1).WillOnce(Return((osMutexId) 1));

    UdpDriver udpDriverUnderTest(ZERO_IP_ADDR, ZERO_IP_ADDR, (u16_t) 0, (u16_t) 0,
            &udp_if, &os_if, TEST_SIGNAL, TEST_THREAD_ID);
    ASSERT_TRUE(udpDriverUnderTest.initialize());
    ASSERT_FALSE(udpDriverUnderTest.setupReceive(nullptr));
    EXPECT_EQ(udpDriverUnderTest.getPcb(), nullptr);
}

TEST_F(UdpDriverTest, FailSetupReceiveOnUdpBind) {
    struct udp_pcb udpPcb;

    EXPECT_CALL(udp_if, udpRemove(_)).Times(1);
    EXPECT_CALL(udp_if, udpBind(_, _, _)).Times(1).WillOnce(Return(ERR_USE));
    EXPECT_CALL(udp_if, udpNew()).Times(1).WillOnce(Return(&udpPcb));
    EXPECT_CALL(os_if, OS_osMutexCreate(_)).Times(1).WillOnce(Return((osMutexId) 1));

    UdpDriver udpDriverUnderTest(ZERO_IP_ADDR, ZERO_IP_ADDR, (u16_t) 0, (u16_t) 0,
            &udp_if, &os_if, TEST_SIGNAL, TEST_THREAD_ID);
    ASSERT_TRUE(udpDriverUnderTest.initialize());
    ASSERT_FALSE(udpDriverUnderTest.setupReceive(nullptr));

    /* Check the PCB has been removed. */
    EXPECT_EQ(udpDriverUnderTest.getPcb(), nullptr);
}

TEST_F(UdpDriverTest, SucceedReceive) {
    struct pbuf rxPbuf;

    uint8_t rxBuff[10] = {};

    EXPECT_CALL(os_if, OS_osMutexWait(_, _)).WillRepeatedly(Return(osOK));

    EXPECT_CALL(udp_if, pbufFree(_)).Times(1);
    EXPECT_CALL(udp_if, pbufCopyPartial(_, _, _, _)).Times(1).WillOnce(Return((u16_t) 1));
    EXPECT_CALL(os_if, OS_osSignalWait(_, _)).Times(1);
    EXPECT_CALL(os_if, OS_osMutexCreate(_)).Times(1).WillOnce(Return((osMutexId) 1));

    UdpDriver udpDriverUnderTest(ZERO_IP_ADDR, ZERO_IP_ADDR, (u16_t) 0, (u16_t) 0,
            &udp_if, &os_if, TEST_SIGNAL, TEST_THREAD_ID);
    ASSERT_TRUE(udpDriverUnderTest.initialize());

    udpDriverUnderTest.setRecvPbuf(&rxPbuf);

    ASSERT_TRUE(udpDriverUnderTest.receive(rxBuff, sizeof(rxBuff)));
}

TEST_F(UdpDriverTest, FailReceiveRxArrayNull) {
    struct pbuf rxPbuf;

    EXPECT_CALL(os_if, OS_osMutexWait(_, _)).WillRepeatedly(Return(osOK));

    EXPECT_CALL(os_if, OS_osSignalWait(_, _)).Times(1);
    EXPECT_CALL(os_if, OS_osMutexCreate(_)).Times(1).WillOnce(Return((osMutexId) 1));

    UdpDriver udpDriverUnderTest(ZERO_IP_ADDR, ZERO_IP_ADDR, (u16_t) 0, (u16_t) 0,
            &udp_if, &os_if, TEST_SIGNAL, TEST_THREAD_ID);
    ASSERT_TRUE(udpDriverUnderTest.initialize());

    udpDriverUnderTest.setRecvPbuf(&rxPbuf);

    ASSERT_FALSE(udpDriverUnderTest.receive(NULL, 10));
}

TEST_F(UdpDriverTest, FailReceiveRxPbufNull) {
    uint8_t rxBuff[10] = {};

    EXPECT_CALL(os_if, OS_osMutexWait(_, _)).WillRepeatedly(Return(osOK));

    EXPECT_CALL(os_if, OS_osSignalWait(_, _)).Times(1);
    EXPECT_CALL(os_if, OS_osMutexCreate(_)).Times(1).WillOnce(Return((osMutexId) 1));

    UdpDriver udpDriverUnderTest(ZERO_IP_ADDR, ZERO_IP_ADDR, (u16_t) 0, (u16_t) 0,
            &udp_if, &os_if, TEST_SIGNAL, TEST_THREAD_ID);
    ASSERT_TRUE(udpDriverUnderTest.initialize());

    udpDriverUnderTest.setRecvPbuf(NULL);

    ASSERT_FALSE(udpDriverUnderTest.receive(rxBuff, sizeof(rxBuff)));
}

TEST_F(UdpDriverTest, FailReceiveZeroBytesCopied) {
    uint8_t rxBuff[10] = {};
    struct pbuf rxPbuf;

    EXPECT_CALL(os_if, OS_osMutexWait(_, _)).WillRepeatedly(Return(osOK));

    EXPECT_CALL(udp_if, pbufCopyPartial(_, _, _, _)).Times(1).WillOnce(Return((u16_t) 0));
    EXPECT_CALL(os_if, OS_osSignalWait(_, _)).Times(1);
    EXPECT_CALL(os_if, OS_osMutexCreate(_)).Times(1).WillOnce(Return((osMutexId) 1));

    UdpDriver udpDriverUnderTest(ZERO_IP_ADDR, ZERO_IP_ADDR, (u16_t) 0, (u16_t) 0,
            &udp_if, &os_if, TEST_SIGNAL, TEST_THREAD_ID);
    ASSERT_TRUE(udpDriverUnderTest.initialize());

    udpDriverUnderTest.setRecvPbuf(&rxPbuf);

    ASSERT_FALSE(udpDriverUnderTest.receive(rxBuff, sizeof(rxBuff)));
}

TEST_F(UdpDriverTest, SucceedTransmit) {
    uint8_t txBuff[10] = {};
    struct pbuf txPbuf;
    const u32_t TEST_IP_ADDR = 0xC0A80008;
    const u32_t TEST_IP_ADDR_PC = 0xC0A80002;

    EXPECT_CALL(udp_if, pbufFree(&txPbuf)).Times(1).WillOnce(Return((u8_t) 1));
    EXPECT_CALL(udp_if, udpDisconnect(_)).Times(1);
    EXPECT_CALL(udp_if, udpSend(_, _)).Times(1).WillOnce(Return(ERR_OK));
    EXPECT_CALL(udp_if, udpConnect(_, _, _)).Times(1).WillOnce(Return(ERR_OK));
    EXPECT_CALL(udp_if, pbufTake(_, _, _)).Times(1).WillOnce(Return(ERR_OK));
    EXPECT_CALL(udp_if, pbufAlloc(PBUF_TRANSPORT, _, PBUF_RAM)).Times(1).WillOnce(Return(&txPbuf));
    EXPECT_CALL(os_if, OS_osMutexCreate(_)).Times(1).WillOnce(Return((osMutexId) 1));

    UdpDriver udpDriverUnderTest(TEST_IP_ADDR, TEST_IP_ADDR_PC, (u16_t) 7,
            (u16_t) 6340, &udp_if, &os_if, TEST_SIGNAL, TEST_THREAD_ID);
    ASSERT_TRUE(udpDriverUnderTest.initialize());

    ASSERT_TRUE(udpDriverUnderTest.transmit(txBuff, sizeof(txBuff)));
}

TEST_F(UdpDriverTest, FailTransmitWhenTxBuffNull) {
    struct pbuf txPbuf;
    const u32_t TEST_IP_ADDR = 0xC0A80008;
    const u32_t TEST_IP_ADDR_PC = 0xC0A80002;

    EXPECT_CALL(udp_if, pbufTake(_, _, _)).Times(0);
    EXPECT_CALL(udp_if, pbufAlloc(PBUF_TRANSPORT, _, PBUF_RAM)).Times(1).WillOnce(Return(&txPbuf));
    EXPECT_CALL(os_if, OS_osMutexCreate(_)).Times(1).WillOnce(Return((osMutexId) 1));

    UdpDriver udpDriverUnderTest(TEST_IP_ADDR, TEST_IP_ADDR_PC, (u16_t) 7,
            (u16_t) 6340, &udp_if, &os_if, TEST_SIGNAL, TEST_THREAD_ID);
    ASSERT_TRUE(udpDriverUnderTest.initialize());

    ASSERT_FALSE(udpDriverUnderTest.transmit(nullptr, 10));
}

TEST_F(UdpDriverTest, FailTransmitWhenPbufAllocUnsuccessful) {
    uint8_t txBuff[10] = {};
    const u32_t TEST_IP_ADDR = 0xC0A80008;
    const u32_t TEST_IP_ADDR_PC = 0xC0A80002;

    EXPECT_CALL(udp_if, pbufFree(_)).Times(0);
    EXPECT_CALL(udp_if, pbufAlloc(PBUF_TRANSPORT, _, PBUF_RAM)).Times(1).WillOnce(Return((struct pbuf *) NULL));
    EXPECT_CALL(os_if, OS_osMutexCreate(_)).Times(1).WillOnce(Return((osMutexId) 1));

    UdpDriver udpDriverUnderTest(TEST_IP_ADDR, TEST_IP_ADDR_PC, (u16_t) 7,
            (u16_t) 6340, &udp_if, &os_if, TEST_SIGNAL, TEST_THREAD_ID);
    ASSERT_TRUE(udpDriverUnderTest.initialize());

    ASSERT_FALSE(udpDriverUnderTest.transmit(txBuff, sizeof(txBuff)));
}

TEST_F(UdpDriverTest, FailTransmitWhenPbufTakeUnsuccessful) {
    uint8_t txBuff[10] = {};
    struct pbuf txPbuf;
    const u32_t TEST_IP_ADDR = 0xC0A80008;
    const u32_t TEST_IP_ADDR_PC = 0xC0A80002;

    EXPECT_CALL(udp_if, pbufTake(_, _, _)).Times(1).WillOnce(Return(ERR_MEM));
    EXPECT_CALL(udp_if, pbufAlloc(PBUF_TRANSPORT, _, PBUF_RAM)).Times(1).WillOnce(Return(&txPbuf));
    EXPECT_CALL(os_if, OS_osMutexCreate(_)).Times(1).WillOnce(Return((osMutexId) 1));

    UdpDriver udpDriverUnderTest(TEST_IP_ADDR, TEST_IP_ADDR_PC, (u16_t) 7,
            (u16_t) 6340, &udp_if, &os_if, TEST_SIGNAL, TEST_THREAD_ID);
    ASSERT_TRUE(udpDriverUnderTest.initialize());

    ASSERT_FALSE(udpDriverUnderTest.transmit(txBuff, sizeof(txBuff)));
}

TEST_F(UdpDriverTest, FailTransmitWhenUdpConnectUnsuccessful) {
    uint8_t txBuff[10] = {};
    struct pbuf txPbuf;
    const u32_t TEST_IP_ADDR = 0xC0A80008;
    const u32_t TEST_IP_ADDR_PC = 0xC0A80002;

    EXPECT_CALL(udp_if, udpConnect(_, _, _)).Times(1).WillOnce(Return(ERR_VAL));
    EXPECT_CALL(udp_if, pbufTake(_, _, _)).Times(1).WillOnce(Return(ERR_OK));
    EXPECT_CALL(udp_if, pbufAlloc(PBUF_TRANSPORT, _, PBUF_RAM)).Times(1).WillOnce(Return(&txPbuf));
    EXPECT_CALL(os_if, OS_osMutexCreate(_)).Times(1).WillOnce(Return((osMutexId) 1));

    UdpDriver udpDriverUnderTest(TEST_IP_ADDR, TEST_IP_ADDR_PC, (u16_t) 7,
            (u16_t) 6340, &udp_if, &os_if, TEST_SIGNAL, TEST_THREAD_ID);
    ASSERT_TRUE(udpDriverUnderTest.initialize());

    ASSERT_FALSE(udpDriverUnderTest.transmit(txBuff, sizeof(txBuff)));
}

TEST_F(UdpDriverTest, FailTransmitWhenUdpSendUnsuccessful) {
    uint8_t txBuff[10] = {};
    struct pbuf txPbuf;
    const u32_t TEST_IP_ADDR = 0xC0A80008;
    const u32_t TEST_IP_ADDR_PC = 0xC0A80002;

    EXPECT_CALL(udp_if, udpDisconnect(_)).Times(1);
    EXPECT_CALL(udp_if, udpSend(_, _)).Times(1).WillOnce(Return(ERR_VAL));
    EXPECT_CALL(udp_if, udpConnect(_, _, _)).Times(1).WillOnce(Return(ERR_OK));
    EXPECT_CALL(udp_if, pbufTake(_, _, _)).Times(1).WillOnce(Return(ERR_OK));
    EXPECT_CALL(udp_if, pbufAlloc(PBUF_TRANSPORT, _, PBUF_RAM)).Times(1).WillOnce(Return(&txPbuf));
    EXPECT_CALL(os_if, OS_osMutexCreate(_)).Times(1).WillOnce(Return((osMutexId) 1));

    UdpDriver udpDriverUnderTest(TEST_IP_ADDR, TEST_IP_ADDR_PC, (u16_t) 7,
            (u16_t) 6340, &udp_if, &os_if, TEST_SIGNAL, TEST_THREAD_ID);
    ASSERT_TRUE(udpDriverUnderTest.initialize());

    ASSERT_FALSE(udpDriverUnderTest.transmit(txBuff, sizeof(txBuff)));
}

/**
 * @}
 */
/* end - UdpDriver_Test */
