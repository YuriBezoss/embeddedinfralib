#include "gmock/gmock.h"
#include "hal/windows/SynchronousRandomDataGeneratorWin.hpp"
#include "infra/event/test_helper/EventDispatcherWithWeakPtrFixture.hpp"
#include "infra/util/test_helper/MockHelpers.hpp"
#include "mbedtls/config.h"
#include "mbedtls/certs.h"
#include "services/network/ConnectionMbedTls.hpp"
#include "services/network/test_doubles/ConnectionLoopBack.hpp"
#include "services/network/test_doubles/ConnectionMock.hpp"
#include "services/network/test_doubles/ConnectionStub.hpp"

class ConnectionMbedTlsTest
    : public testing::Test
    , public infra::EventDispatcherWithWeakPtrFixture
{
public:
    ConnectionMbedTlsTest()
        : connectionFactory(network, serverCertificates, randomDataGenerator)
        , thisListener(infra::UnOwnedSharedPtr(*this))
    {
        serverCertificates.AddCertificateAuthority(infra::BoundedConstString(mbedtls_test_cas_pem, mbedtls_test_cas_pem_len));
        serverCertificates.AddOwnCertificate(infra::BoundedConstString(mbedtls_test_srv_crt, mbedtls_test_srv_crt_len), infra::BoundedConstString(mbedtls_test_srv_key, mbedtls_test_srv_key_len));

        clientCertificates.AddCertificateAuthority(infra::BoundedConstString(mbedtls_test_cas_pem, mbedtls_test_cas_pem_len));
        clientCertificates.AddOwnCertificate(infra::BoundedConstString(mbedtls_test_cli_crt, mbedtls_test_cli_crt_len), infra::BoundedConstString(mbedtls_test_cli_key, mbedtls_test_cli_key_len));
    }

    services::ZeroCopyServerConnectionObserverFactoryMock serverObserverFactory;
    services::ZeroCopyClientConnectionObserverFactoryMock clientObserverFactory;
    testing::StrictMock<services::ZeroCopyConnectionFactoryMock> network;
    services::ConnectionLoopBackFactory loopBackNetwork;
    hal::SynchronousRandomDataGeneratorWin randomDataGenerator;
    infra::SharedPtr<void> thisListener;
    services::ZeroCopyServerConnectionObserverFactory* mbedTlsObserverFactory;
    services::ZeroCopyConnection* mbedTlsConnection;
    services::MbedTlsCertificates serverCertificates;
    services::MbedTlsCertificates clientCertificates;
    services::ConnectionFactoryMbedTls::WithMaxConnectionsListenersAndConnectors<2, 1, 1> connectionFactory;
};

TEST_F(ConnectionMbedTlsTest, when_allocation_on_network_fails_Listen_returns_nullptr)
{
    EXPECT_CALL(network, ListenMock(1234)).WillOnce(testing::Return(nullptr));
    infra::SharedPtr<void> listener = connectionFactory.Listen(1234, serverObserverFactory);
    EXPECT_EQ(nullptr, listener);
}

TEST_F(ConnectionMbedTlsTest, when_listener_allocation_fails_Listen_returns_nullptr)
{
    EXPECT_CALL(network, ListenMock(1234)).WillOnce(testing::Return(thisListener));
    infra::SharedPtr<void> listener1 = connectionFactory.Listen(1234, serverObserverFactory);
    infra::SharedPtr<void> listener2 = connectionFactory.Listen(1234, serverObserverFactory);
    EXPECT_EQ(nullptr, listener2);
}

TEST_F(ConnectionMbedTlsTest, Listen_returns_listener)
{
    EXPECT_CALL(network, ListenMock(1234)).WillOnce(testing::Return(thisListener));
    infra::SharedPtr<void> listener = connectionFactory.Listen(1234, serverObserverFactory);
    EXPECT_NE(nullptr, listener);
}

TEST_F(ConnectionMbedTlsTest, create_connection)
{
    services::ConnectionFactoryMbedTls::WithMaxConnectionsListenersAndConnectors<2, 1, 0> tlsNetworkServer(loopBackNetwork, serverCertificates, randomDataGenerator);
    services::ConnectionFactoryMbedTls::WithMaxConnectionsListenersAndConnectors<2, 0, 1> tlsNetworkClient(loopBackNetwork, clientCertificates, randomDataGenerator);
    infra::SharedPtr<void> listener = tlsNetworkServer.Listen(1234, serverObserverFactory);
    infra::SharedPtr<void> connector = tlsNetworkClient.Connect(services::IPv4Address(), 1234, clientObserverFactory);
    
    infra::SharedOptional<services::ZeroCopyConnectionObserverMock> observer1;
    infra::SharedOptional<services::ZeroCopyConnectionObserverMock> observer2;
    EXPECT_CALL(serverObserverFactory, ConnectionAccepted(testing::_))
        .WillOnce(infra::Lambda([&](services::ZeroCopyConnection& connection) { return observer1.Emplace(connection); }));
    EXPECT_CALL(clientObserverFactory, ConnectionEstablished(testing::_))
        .WillOnce(infra::Lambda([&](services::ZeroCopyConnection& connection) { return observer2.Emplace(connection); }));
    ExecuteAllActions();
    observer1->Subject().AbortAndDestroy();
}

TEST_F(ConnectionMbedTlsTest, send_and_receive_data)
{
    services::ConnectionFactoryMbedTls::WithMaxConnectionsListenersAndConnectors<2, 1, 0> tlsNetworkServer(loopBackNetwork, serverCertificates, randomDataGenerator);
    services::ConnectionFactoryMbedTls::WithMaxConnectionsListenersAndConnectors<2, 0, 1> tlsNetworkClient(loopBackNetwork, clientCertificates, randomDataGenerator);
    infra::SharedPtr<void> listener = tlsNetworkServer.Listen(1234, serverObserverFactory);
    infra::SharedPtr<void> connector = tlsNetworkClient.Connect(services::IPv4Address(), 1234, clientObserverFactory);

    infra::SharedOptional<services::ZeroCopyConnectionObserverStub> observer1;
    infra::SharedOptional<services::ZeroCopyConnectionObserverStub> observer2;
    EXPECT_CALL(serverObserverFactory, ConnectionAccepted(testing::_))
        .WillOnce(infra::Lambda([&](services::ZeroCopyConnection& connection) { return observer1.Emplace(connection); }));
    EXPECT_CALL(clientObserverFactory, ConnectionEstablished(testing::_))
        .WillOnce(infra::Lambda([&](services::ZeroCopyConnection& connection) { return observer2.Emplace(connection); }));
    ExecuteAllActions();

    observer2->SendData(std::vector<uint8_t>{ 1, 2, 3, 4 });
    ExecuteAllActions();
    EXPECT_EQ((std::vector<uint8_t>{ 1, 2, 3, 4 }), observer1->receivedData);

    observer1->SendData(std::vector<uint8_t>{ 5, 6, 7, 8 });
    ExecuteAllActions();
    EXPECT_EQ((std::vector<uint8_t>{ 5, 6, 7, 8 }), observer2->receivedData);

    observer1->Subject().AbortAndDestroy();
}
