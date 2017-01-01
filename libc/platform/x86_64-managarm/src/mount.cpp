
#include <string.h>
#include <errno.h>
#include <sys/mount.h>

#include <mlibc/ensure.h>
#include <mlibc/cxx-support.hpp>
#include <mlibc/frigg-alloc.hpp>
#include <mlibc/posix-pipe.hpp>

#include <posix.frigg_pb.hpp>

int mount(const char *source, const char *target,
		const char *fstype, unsigned long flags, const void *data) {
	HelAction actions[3];
	HelSimpleResult *offer;
	HelSimpleResult *send_req;
	HelInlineResult *recv_resp;

	globalQueue.trim();

	managarm::posix::CntRequest<MemoryAllocator> req(getAllocator());
	req.set_request_type(managarm::posix::CntReqType::MOUNT);
	req.set_path(frigg::String<MemoryAllocator>(getAllocator(), source));
	req.set_target_path(frigg::String<MemoryAllocator>(getAllocator(), target));

	frigg::String<MemoryAllocator> ser(getAllocator());
	req.SerializeToString(&ser);
	actions[0].type = kHelActionOffer;
	actions[0].flags = kHelItemAncillary;
	actions[1].type = kHelActionSendFromBuffer;
	actions[1].flags = kHelItemChain;
	actions[1].buffer = ser.data();
	actions[1].length = ser.size();
	actions[2].type = kHelActionRecvInline;
	actions[2].flags = 0;
	HEL_CHECK(helSubmitAsync(kHelThisThread, actions, 3,
			globalQueue.getQueue(), 0));

	offer = (HelSimpleResult *)globalQueue.dequeueSingle();
	send_req = (HelSimpleResult *)globalQueue.dequeueSingle();
	recv_resp = (HelInlineResult *)globalQueue.dequeueSingle();

	HEL_CHECK(offer->error);
	HEL_CHECK(send_req->error);
	HEL_CHECK(recv_resp->error);

	managarm::posix::SvrResponse<MemoryAllocator> resp(getAllocator());
	resp.ParseFromArray(recv_resp->data, recv_resp->length);
	assert(resp.error() == managarm::posix::Errors::SUCCESS);
	return 0;
}
