#ifndef NET_TCPSERVER_ASYNC_SERVER_H
#define NET_TCPSERVER_ASYNC_SERVER_H
/**
 * @file net/tcpserver/AsyncServer.h
 * @author Ian Yang
 * @date Created <2010-05-26 15:23:00>
 * @brief Asynchronous TCP server
 */

#include <boost/asio.hpp>
#include <boost/noncopyable.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/function.hpp>
#include <boost/bind.hpp>

namespace tcpserver {

template<typename ConnectionFactory>
class AsyncServer : private boost::noncopyable
{
public:
    typedef typename ConnectionFactory::connection_type connection_type;
    typedef boost::shared_ptr<connection_type> connection_ptr;

    typedef boost::function1<void, const boost::system::error_code&> error_handler;

    /**
     * @brief Construct a server listening on the specified TCP address and
     * port.
     * @param address listen address
     * @param port listen port
     */
    explicit AsyncServer(
        const std::string& address,
        const std::string& port,
        const boost::shared_ptr<ConnectionFactory>& connectionFactory
    );

    /// @brief Run main loop.
    /// To handle connection in threads pool, please run this method in all
    /// threads.
    ///
    /// e.g.
    ///
    /// <code>
    /// AsyncServer<Factory> server(address, port, factory);
    /// boost::thread_group threads;
    /// std::size_t poolSize = 10;
    /// for (std::size_t i = 0; i < poolSize; ++i) {
    ///     threads.create_thread(boost::bind(&AsyncServer<Factory>::run, &server));
    /// }
    /// threads.join_all();
    /// </code>
    inline void run();

    /// @brief Stop the server.
    inline void stop();

    /// @brief Set a handler to process error while listening.
    inline void setErrorHandler(error_handler handler);

private:
    inline void listen(const std::string& address, const std::string& port);

    inline void asyncAccept();

    /// @brief Accept a incoming connection asynchronously.
    void onAccept(const boost::system::error_code& e);

    /// @brief Handle error
    inline void onError(const boost::system::error_code& e);

    /// @brief The io_service used to perform asynchronous operations.
    boost::asio::io_service ioService_;

    /// @brief Acceptor used to listen for incoming connections.
    boost::asio::ip::tcp::acceptor acceptor_;

    /// @brief Factory used to create new connection handler
    boost::shared_ptr<ConnectionFactory> connectionFactory_;

    /// @brief The next connection to be accepted.
    connection_ptr newConnection_;

    error_handler errorHandler_;
};

template<typename ConnectionFactory>
AsyncServer<ConnectionFactory>::AsyncServer(
    const std::string& address,
    const std::string& port,
    const boost::shared_ptr<ConnectionFactory>& connectionFactory
)
: ioService_()
, acceptor_(ioService_)
, connectionFactory_(connectionFactory)
, newConnection_()
, errorHandler_()
{
    listen(address, port);
    asyncAccept();
}

template<typename ConnectionFactory>
void AsyncServer<ConnectionFactory>::run()
{
    ioService_.run();
}

template<typename ConnectionFactory>
void AsyncServer<ConnectionFactory>::stop()
{
    ioService_.stop();
}

template<typename ConnectionFactory>
void AsyncServer<ConnectionFactory>::setErrorHandler(error_handler handler)
{
    errorHandler_ = handler;
}

template<typename ConnectionFactory>
void AsyncServer<ConnectionFactory>::listen(const std::string& address,
                                            const std::string& port)
{
    boost::asio::ip::tcp::resolver resolver(ioService_);
    boost::asio::ip::tcp::resolver::query query(address, port);
    boost::asio::ip::tcp::endpoint endpoint = *resolver.resolve(query);
    acceptor_.open(endpoint.protocol());
    acceptor_.set_option(boost::asio::ip::tcp::acceptor::reuse_address(true));
    acceptor_.bind(endpoint);
    acceptor_.listen();
}

template<typename ConnectionFactory>
void AsyncServer<ConnectionFactory>::asyncAccept()
{
    newConnection_.reset(
        connectionFactory_->create(ioService_)
    );
    acceptor_.async_accept(
        newConnection_->socket(),
        boost::bind(
            &AsyncServer<ConnectionFactory>::onAccept,
            this,
            boost::asio::placeholders::error
        )
    );

}

template<typename ConnectionFactory>
void AsyncServer<ConnectionFactory>::onAccept(const boost::system::error_code& e)
{
    if (!e)
    {
        newConnection_->start();

        asyncAccept();
    }
    else
    {
        onError(e);
    }
}

template<typename ConnectionFactory>
void AsyncServer<ConnectionFactory>::onError(const boost::system::error_code& e)
{
    if (errorHandler_)
    {
        errorHandler_(e);
    }
}

} // namespace sf1v5

#endif // NET_TCPSERVER_ASYNC_SERVER_H