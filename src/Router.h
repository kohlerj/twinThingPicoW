
#ifndef ROUTER_H_
#define ROUTER_H_

#include "CommInterface.h"

/***
 * An interface class that defines the behavior of a Router
 */
class Router {
 public:
  Router(){};
  virtual ~Router(){};

  /***
   * Use interface to set subscriptions for the router
   * @param interface
   */
  virtual void Subscribe(CommInterface *interface) = 0;

  /***
   * Route a message. Handle the message and connect to wider application
   * @param topic - non zero terminated string
   * @param topic_len - length of topic
   * @param payload - memory structure of payload
   * @param payload_len - payload length
   * @param interface - MQTT interface to use for any response publication
   */
  virtual void Route(const char *topic, size_t topic_len, const void *payload,
                     size_t payload_len, CommInterface *interface) = 0;
};

#endif /* ROUTER_H_ */
