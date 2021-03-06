#include <simcode/redis/redis.h>
#include <vector>
using namespace std;
using namespace simcode;
using namespace simcode::redis;

void Redis::swap(Redis& h)
{
    ::swap(errcode_, h.errcode_);
    ::swap(ctx_, h.ctx_);
    errmsg_.swap(h.errmsg_);
    info_.swap(h.info_);
}

int Redis::Connect(void)
{
    if (ctx_)
        return 0;
    if (info_.host.length() < 5 || info_.port < 1024)
    {
        char tmp[50] = {0};
        sprintf(tmp, "host:%s|port:%d", info_.host.c_str(), info_.port);
        errmsg_ = std::string(tmp);
        return 1;//已经初始化过
    }

    errcode_ = 0;
    errmsg_.clear();
    ctx_ = redisConnectWithTimeout(info_.host.c_str(),
                                   info_.port,
                                   info_.time_out);

    if (ctx_->err)
    {
        errcode_ = ctx_->err;
        errmsg_ = "ctxt:" + std::string(ctx_->errstr);
        redisFree(ctx_);
        ctx_ = NULL;
        return 2;
    }
    return 0;
}

redisReply* Redis::Command(const char* format, ...)
{
    va_list argptr;
    va_start(argptr,format);
    redisReply *reply = vCommand(format,argptr);
    va_end(argptr);
    return reply;
}

int Redis::AppendCommand(const char* format, ...)
{
//    if (NULL == ctx_)
//    {
//        //if (Connect())
//        return errcode_;
//    }
//    if (ctx_->err)
//    {
//        errcode_ = ctx_->err;
//        errmsg_ = "ctxt:" + std::string(ctx_->errstr);
//        return errcode_;
//    }
    va_list argptr;
    va_start(argptr,format);
    int ret = ::redisvAppendCommand(ctx_, format, argptr);
    va_end(argptr);

    return ret;
}

redisReply* Redis::GetReply()
{
//    if (NULL == ctx_)
//    {
//        return NULL;
//    }
//    if (ctx_->err)
//    {
//        errcode_ = ctx_->err;
//        errmsg_ = "ctxt:" + std::string(ctx_->errstr);
//        return NULL;
//    }
    redisReply* r;
    int ret = ::redisGetReply(ctx_, (void**)&r);
    if (ret)
    {
        if (ctx_->err)
        {
            errcode_ = ctx_->err;
            errmsg_ = "ctxt:" + std::string(ctx_->errstr);
        }
        if (r)
        {
            errcode_ = r->integer;
            errmsg_ = "reply:" + std::string(r->str);
        }
        return NULL;
    }
    return r;
}

int Redis::GetAllReply(size_t num, std::vector<redisReply* >& rp_vec)
{
//    if (NULL == ctx_)
//        return 1;
//    if (ctx_->err)
//    {
//        errcode_ = ctx_->err;
//        errmsg_ = "ctxt:" + std::string(ctx_->errstr);
//        return 2;
//    }
    for (size_t i=0; i<num; ++i)
    {
        redisReply *r = NULL;
        redisGetReply(ctx_, (void **)&r);
        rp_vec.push_back(r);
    }

    if (ctx_->err)
    {
        errcode_=ctx_->err;
        errmsg_="ctxt:"+std::string(ctx_->errstr);
    }

    return 0;
}

redisReply* Redis::vCommand(const char* format, va_list argptr)
{
    //如果没有连接或连接错误 并且重连失败，则直接返回
    //if ((!ctx_ || ctx_->err) && Update()) return NULL;
    return static_cast<redisReply*>(redisvCommand(ctx_, format, argptr));
}

redisReply* Redis::CommandArgv(const std::vector<std::string>& argvec)
{
    //if ((!ctx_ || ctx_->err) && Update()) return NULL;
    int argc = argvec.size();
    vector<const char *> argv;
    vector<size_t> argvlen;
    argv.reserve(argc);
    argvlen.reserve(argc);
    vector<string>::const_iterator it;
    for (it=argvec.begin(); it!=argvec.end(); ++it)
    {
        argv.push_back(it->c_str());
        argvlen.push_back(it->length());
    }
    return static_cast<redisReply *>(redisCommandArgv(ctx_,
                                          argc, argv.data(), argvlen.data()));
}

redisReply *Redis::CommandArgvPrev(int argc, const char **argv, const size_t *argvlen)
{
    //if ((!ctx_ || ctx_->err) && Update()) return NULL;
    return static_cast<redisReply *>(redisCommandArgv(ctx_,argc, argv, argvlen));
}

int Redis::AppendCommandArgv(const std::vector<std::string>& argvec)
{
    //if ((!ctx_ || ctx_->err) && Update()) return 0;
    int argc = argvec.size();
    vector<const char *> argv;
    vector<size_t> argvlen;
    argv.reserve(argc);
    argvlen.reserve(argc);
    vector<string>::const_iterator it;
    for (it=argvec.begin(); it!=argvec.end(); ++it)
    {
        argv.push_back(it->c_str());
        argvlen.push_back(it->length());
    }
    return redisAppendCommandArgv(ctx_, argc, argv.data(), argvlen.data());
}

std::vector<redisReply*>
Redis::AppendArgvs(const std::vector<std::vector<std::string> >& argvs)
{
    vector<redisReply*> rs;
    //if ((!ctx_ || ctx_->err) && Update()) return rs;
    vector<vector<string> >::const_iterator it;
    for (it=argvs.begin(); it!=argvs.end(); ++it)
    {
        int ret = 0;
        ret=AppendCommandArgv(*it);
        if(ret)
        {
            if (ctx_->err)
            {
                errcode_ = ctx_->err;
                errmsg_ = "ctxt:" + std::string(ctx_->errstr);
                continue;
            }
            errcode_ = ret;
            errmsg_ = "append failed";
        }
    }
    for (size_t j=0; j<argvs.size(); ++j)
    {
        int ret = 0;
        redisReply *r = NULL;
        ret=redisGetReply(ctx_, (void **)&r);
        if(ret)
        {
            if (ctx_->err)
            {
                errcode_ = ctx_->err;
                errmsg_ = "ctxt:" + std::string(ctx_->errstr);
                continue;
            }
            errcode_ = ret;
            errmsg_ = "getreply failed";
        }
        rs.push_back(r);
    }
    return rs;
}

redisReply* Redis::TryCommand(const char* format, ...)
{
    if ((!ctx_ || ctx_->err) && Update()) return NULL;
    redisReply* r = NULL;
    va_list argptr;
    va_start(argptr,format);
    r = vTryCommand(format, argptr);
    va_end(argptr);
    return r;

}

redisReply* Redis::vTryCommand(const char* format, va_list argptr)
{
    if ((!ctx_ || ctx_->err) && Update()) return NULL;
    redisReply* r = NULL;
    for (int i=0; i<2; i++)
    {
        r = vCommand(format,argptr);
        if (r&&r->type!=REDIS_REPLY_ERROR) break;
        if (0==i && Update()) break;
    }
    return r;
}

redisReply* Redis::TryCommandArgv(const std::vector<std::string>& argvec)
{
    if ((!ctx_ || ctx_->err) && Update()) return NULL;
    redisReply* r=NULL;
    int argc = argvec.size();
    vector<const char *> argv;
    vector<size_t> argvlen;
    argv.reserve(argc);
    argvlen.reserve(argc);
    vector<string>::const_iterator it;
    for (it=argvec.begin(); it!=argvec.end(); ++it)
    {
        argv.push_back(it->c_str());
        argvlen.push_back(it->length());
    }

    for (int i=0; i<2; i++)
    {
        r = static_cast<redisReply*>(redisCommandArgv(ctx_,argc, argv.data(), argvlen.data()));
        if (r&&r->type!=REDIS_REPLY_ERROR) break;
        if (0==i && Update()) break;
    }
    return r;
}

std::vector<redisReply*>
Redis::TryAppendArgvs(const std::vector<std::vector<std::string> >& argvs)
{
    vector<redisReply*> rs;
    if ((!ctx_ || ctx_->err) && Update()) return rs;
    for (int i=0; i<2; i++)
    {
        rs = AppendArgvs(argvs);
        if (!errcode_) break;
        if (0==i && Update()) break;
    }
    return rs;
}

void Redis::Reset(const std::string& _host,
                  const int _port,
                  const struct timeval& _time_out)
{
    info_.host = _host;
    info_.port = _port;
    info_.time_out = _time_out;
    DisConnect();
}

void Redis::DisConnect()
{
    if (ctx_)
        redisFree(ctx_);
    ctx_ = NULL;
    errcode_ = 1;
    errmsg_.clear();
}
