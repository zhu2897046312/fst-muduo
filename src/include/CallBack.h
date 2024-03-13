#pragma once

#include "Buffer.h"
#include "TcpConnection.h"
#include "Timestamp.h"
#include <functional>
#include <memory>
namespace fst {

class TcpConnection;

using TcpConnectionPtr = std::shared_ptr<TcpConnection>;
using ConnectionCallBack= std::function<void(const TcpConnectionPtr&)>;
using CloseCallBack = std::function<void(const TcpConnectionPtr&)>;
using WriteCompleteCallBack = std::function<void (const TcpConnectionPtr&)>;

using MessageCallBack = std::function<void(const  TcpConnectionPtr&
                        , Buffer*
                        , Timestamp)>;

using HightWaterMarkCallBack = std::function<void(const TcpConnectionPtr&,size_t)>;

}