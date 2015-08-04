/*
 * Copyright (c) 2013 Samsung Electronics Co., Ltd. All rights reserved.
 *
 * Licensed under the Flora License, Version 1.1 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://floralicense.org/license/
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <stdio.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>

#define PATH_MAX 255

int main(void)
{
   const char *base = "/elm_indicator-0";
   char file[PATH_MAX];
   int ret0 = 0, ret1 = 0;

   //create 1st shm
   snprintf(file, sizeof(file), "%s.%s", base, "0");
   int fd0 = shm_open(file, O_RDWR | O_CREAT | O_EXCL, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
   if (fd0 >= 0)
     {
        int flags = fcntl(fd0, F_GETFL);
        if (flags != -1)
          {
             flags &= ~FD_CLOEXEC;
             ret0 = fcntl(fd0, F_SETFL, flags);
          }
        else
          {
             ret0 = -1;
          }
     }

   //create 2nd shm
   snprintf(file, sizeof(file), "%s.%s", base, "1");
   int fd1 = shm_open(file, O_RDWR | O_CREAT | O_EXCL, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
   if (fd1 >= 0)
     {
        int flags = fcntl(fd1, F_GETFL);
        if (flags != -1)
          {
             flags &= ~FD_CLOEXEC;
             ret1 = fcntl(fd1, F_SETFL, flags);
          }
        else
          {
             ret1 = -1;
          }
     }

   if (fd0 < 0 || fd1 < 0 || ret0 < 0 || ret1 < 0)
     return -1;

   return 0;
}
