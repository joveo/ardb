/*
 *Copyright (c) 2013-2013, yinqiwen <yinqiwen@gmail.com>
 *All rights reserved.
 *
 *Redistribution and use in source and binary forms, with or without
 *modification, are permitted provided that the following conditions are met:
 *
 *  * Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *  * Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *  * Neither the name of Redis nor the names of its contributors may be used
 *    to endorse or promote products derived from this software without
 *    specific prior written permission.
 *
 *THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 *AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 *IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 *ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS
 *BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 *CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 *SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 *INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 *CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 *ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
 *THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef RDB_HPP_
#define RDB_HPP_
#include <string>
#include "common.hpp"
#include "buffer/buffer_helper.hpp"
#include "context.hpp"

namespace ardb
{
    typedef int DumpRoutine(void* cb);

    class Ardb;
    class Context;
    class DataDumpFile
    {
        protected:
            FILE* m_read_fp;
            FILE* m_write_fp;
            std::string m_file_path;
            Ardb* m_db;
            uint64 m_cksm;
            DumpRoutine* m_routine_cb;
            void *m_routine_cbdata;
            uint64 m_processed_bytes;
            uint64 m_file_size;
            bool m_is_saving;
            uint32 m_last_save;
            uint64 m_routinetime;
            char* m_read_buf;

            int64 m_expected_data_size;
            int64 m_writed_data_size;
            virtual int DoLoad() = 0;
            virtual int DoSave() = 0;
            bool Read(void* buf, size_t buflen, bool cksm = true);

            int WriteType(uint8 type);
            int WriteKeyType(KeyType type);
            int WriteLen(uint32 len);
            int WriteMillisecondTime(uint64 ts);
            int WriteDouble(double v);
            int WriteLongLongAsStringObject(long long value);
            int WriteRawString(const char *s, size_t len);
            int WriteLzfStringObject(const char *s, size_t len);
            int WriteTime(time_t t);
            int WriteStringObject(const Data& o);

            int ReadType();
            time_t ReadTime();
            int64 ReadMillisecondTime();
            uint32_t ReadLen(int *isencoded);
            bool ReadInteger(int enctype, int64& v);
            bool ReadLzfStringObject(std::string& str);
            bool ReadString(std::string& str);
            int ReadDoubleValue(double&val);
        public:
            DataDumpFile();
            int Init(Ardb* db);
            const std::string& GetPath()
            {
                return m_file_path;
            }
            void SetExpectedDataSize(int64 size);
            int64 DumpLeftDataSize();
            int64 ProcessLeftDataSize();
            int Write(const void* buf, size_t buflen);
            int OpenWriteFile(const std::string& file);
            int OpenReadFile(const std::string& file);
            int Load(uint8 ctx_identity, const std::string& file, DumpRoutine* cb, void *data);
            int Save(const std::string& file, DumpRoutine* cb, void *data);
            int BGSave(const std::string& file);
            uint32 LastSave()
            {
                return m_last_save;
            }
            void Flush();
            void Remove();
            int Rename(const std::string& default_file = "dump.rdb");
            void Close();
            virtual ~DataDumpFile();
    };

    class RedisDumpFile: public DataDumpFile
    {
        private:
            void Close();

            bool LoadObject(Context& ctx, int type, const std::string& key, int64 expiretime);

            void LoadListZipList(Context& ctx, unsigned char* data, const std::string& key, ValueObject& meta_value);
            void LoadHashZipList(Context& ctx, unsigned char* data, const std::string& key, ValueObject& meta_value);
            void LoadZSetZipList(Context& ctx,unsigned char* data, const std::string& key, ValueObject& meta_value);
            void LoadSetIntSet(Context& ctx, unsigned char* data, const std::string& key, ValueObject& meta_value);

            void WriteMagicHeader();

            int DoLoad();
            int DoSave();
        public:
            RedisDumpFile();
            /*
             * return: -1:error 0:not redis dump 1:redis dump file
             */
            static int IsRedisDumpFile(const std::string& file);
            ~RedisDumpFile();
    };

    class ArdbDumpFile: public DataDumpFile
    {
        private:
            Buffer m_write_buffer;
            int WriteMagicHeader();
            int SaveRawKeyValue(const Slice& key, const Slice& value);
            int LoadBuffer(Context& ctx, Buffer& buffer);
            int FlushWriteBuffer();
            int DoLoad();
            int DoSave();
        public:
            ArdbDumpFile();
            ~ArdbDumpFile();
    };

}

#endif /* RDB_HPP_ */