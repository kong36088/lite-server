#ifndef _LITE_FDSET_CONTAINER_H_
#define _LITE_FDSET_CONTAINER_H_

#include "locker.h"
#include <sys/select.h>

class fdset_container{
private:
    fd_set fds;
    rw_lock rwl;

  public:
    fdset_container(){};

    fdset_container(fd_set init_set) : fds(init_set){};

    fd_set get_fds(){
        rwl.read_lock();
        fd_set temp = fds;
        rwl.read_unlock();
        return temp;
    }

    fd_set *get_fds_ptr(){
        rwl.read_lock();
        fd_set *temp_ptr = &fds;
        rwl.read_unlock();
        return temp_ptr;
    }

    void set_fds(fd_set fds){
        rwl.write_lock();
        this->fds = fds;
        rwl.write_unlock();
    }

    void set_fds(fd_set *fds_ptr){
        rwl.write_lock();        
        this->fds = *fds_ptr;
        rwl.write_unlock();
    }
};

#endif