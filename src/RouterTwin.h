
#ifndef ROUTER_TWIN_H_
#define ROUTER_TWIN_H_

#include "TwinTask.h"
#include "Router.h"
#include "State.h"
#include "CommInterface.h"

class RouterTwin : public Router {
 public:
  virtual ~RouterTwin();

  /***
   * Initialise the object give the CommInterface
   * @param comm_inteface
   */
  RouterTwin(CommInterface *comm_interface);

  /***
   * set the TwinTasl
   * @param state
   */
  virtual void SetTwin(TwinTask *twin);

  /***
   * Use the interface to setup all the subscriptions
   * @param comm_interface
   */
  virtual void Subscribe(CommInterface *comm_interface);

  /***
   * Route the message the appropriate part of the application
   * @param topic
   * @param topic_len
   * @param payload
   * @param payload_len
   * @param comm_interface
   */
  virtual void Route(const char *topic, size_t topic_len, const void *payload,
                     size_t payload_len, CommInterface *comm_interface);

 private:
  /***
   * Initialise the object give the Id and CommInterface
   * @param comm_interface = CommInterface
   */
  virtual void InitializeTopics(Comm *comm_interface);

  TwinTask *twin_;

  char *set_topic_ = nullptr;
  char *get_topic_ = nullptr;
};

#endif /* ROUTER_TWIN_H_ */
