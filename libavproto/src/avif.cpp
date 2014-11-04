
#include <openssl/x509.h>
#include <openssl/rsa.h>

#include <boost/shared_ptr.hpp>
#include <boost/scoped_ptr.hpp>
#include <boost/bind.hpp>
#include <boost/thread.hpp>
#include <boost/asio.hpp>
#include <boost/atomic.hpp>

#include <avif.hpp>

#include "protocol/avim-base.pb.h"
#include "async_coro_queue.hpp"

proto::base::avAddress av_address_from_string(std::string av_address)
{
    boost::regex re("([^@]*)@([^/]*)(/.*)?");
    boost::smatch m;
    if(boost::regex_search(av_address, m, re))
    {
        proto::base::avAddress addr;
        addr.set_username(m[1]);
        addr.set_domain(m[2]);
        if(m[3].matched)
        {
            addr.set_resource(m[3]);
        }
        return addr;
    }
    return proto::base::avAddress();
}

std::string av_address_to_string(const proto::base::avAddress & addr)
{
	if(addr.has_resource())
	{
		return boost::str( boost::format("%s@%s/%s") % addr.username() % addr.domain() % addr.resource());
	}
	return boost::str( boost::format("%s@%s") % addr.username() % addr.domain() );
}

void avif::construct()
{
	quitting = boost::make_shared< boost::atomic<bool> >(false);
	_write_queue = boost::make_shared< boost::async_coro_queue< std::queue< std::pair< proto::base::avPacket*, boost::function<void(boost::system::error_code)> > > > >(
		boost::ref(get_io_service())
	);
}

proto::base::avPacket* avif::async_read_packet(boost::asio::yield_context yield_context)
{
    return _impl->async_read_packet(yield_context);
}

bool avif::async_write_packet(proto::base::avPacket* pkt, boost::asio::yield_context yield_context)
{
    return _impl->async_write_packet(pkt, yield_context);
}
