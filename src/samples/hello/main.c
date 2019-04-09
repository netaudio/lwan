/*
 * lwan - simple web server
 * Copyright (c) 2018 Leandro A. F. Pereira <leandro@hardinfo.org>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#include "lwan.h"
#include "hiredis.h"
#include <pthread.h>
#include <adapters/libuv.h>

pthread_mutex_t *mutex = NULL;

LWAN_HANDLER(hello_world)
{
    int result = 0;

    redisContext *c = redisConnect("127.0.0.1", 6379);
    // redisContext *c = redisConnect("192.168.1.204", 6379);
    redisReply *reply = NULL;
    
    // pthread_mutex_lock(mutex);

    if (c == NULL || c->err) {
        result = 1;    
    } else {
        reply = redisCommand(c,"lpop phone");
        if (!reply) {
            result = 1;
        } else {
            redisReply *replypush = NULL;
            printf("reply->str:%s\n", reply->str);
            replypush = redisCommand(c,"rpush phone %s",reply->str);
            if (!replypush) {
                result = 1;
            } else {
                result = 0;
                freeReplyObject(replypush);
            }

            freeReplyObject(reply);
        }
    }

    // unsigned int a = 0;
    // for (int k = 0; k < 100000; ++k) {
    //     for (int i = 0; i < 100000; ++i) {
    //         for (int j = 0; j < 100000; ++j) {
    //             a += 1;
    //         }
    //     }
    // }
    // printf("threadid:%ld,%ld\n", gettid(),a);
    
    if (result == 0) {
        static const char message[] = "SUCCESS!";
        response->mime_type = "text/plain";
        lwan_strbuf_set_static(response->buffer, message, sizeof(message) - 1);        
    
    } else {
        static const char message[] = "ERROR!";
        response->mime_type = "text/plain";
        lwan_strbuf_set_static(response->buffer, message, sizeof(message) - 1);                
        printf("err!!\n");
    }

    if (c) {
        redisFree(c);
    }

    // pthread_mutex_unlock(mutex);
    return HTTP_OK;
}

int
main(void)
{
    const struct lwan_url_map default_map[] = {
        { .prefix = "/", .handler = LWAN_HANDLER_REF(hello_world) },
        { .prefix = NULL }
    };
    struct lwan l;
    mutex = (pthread_mutex_t*)malloc(sizeof(pthread_mutex_t));
    pthread_mutex_init(mutex,NULL);
    if (!mutex) {
        printf("err init mutex\n");
    }
    

    lwan_init(&l);

    lwan_set_url_map(&l, default_map);
    lwan_main_loop(&l);

    lwan_shutdown(&l);

    return 0;
}
