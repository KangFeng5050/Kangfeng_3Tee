#ifndef RT_AVDTrace_H_
#define RT_AVDTrace_H_

#include "common.h"
#include "room.h"

namespace tee3 {
	namespace avd {
		/**
		* AVDTrace接口类
		*
		* 一些基本概念
		* TraceId：一个进程一次启动产生的唯一编号，跟踪整个进程的事件。
		* Span: name --- "start" "end" ....各种事件
		*	span_engine_#1     
		*			|
		*	span_room_roomId    name: "join_getmcu"  "join_signal"  "join_data"
		*			|-----------------|-----------------|
		*		span_audio			span_video			span_screen
		*			|
		*		name:"pub"  "sub"  "stats"  ""
		*		timestamp:
		*		duration: ms
		*		(key-value)s： param, 
		*/
		class RT_API_EXPORT IAVDTrace {
		public:
			struct SpanTag {
				SpanTag(){}
				//
				String key;
				String value;
			};
			typedef std::vector<SpanTag> SpanTagsType;

		public:
			/** 获取AVDTrace对象
			*
			* @note AVDTrace是一个单例对象，一个进程空间只存在一个Trace。
			* @return 返回AVDTrace。
			*/
			static IAVDTrace* Instance();

		public:
			/** 跟踪Trace，设置traceId
			*
			* @param[in] traceId 跟踪编号。
			* @return 返回错误代码。
			* @note 跟踪TraceId每个进程每次引擎初始化后要保持唯一，方便服务器上对此进程的所有Trace做跟踪。
			*/
			virtual Result setTraceId(const String& traceId) = 0;
			/** 发送span信息
			*
			* @param[in] spanId span编号;
			* @param[in] opname 操作或事件名称;
			* @param[in] tags tag列表;
			* @return 返回错误代码。
			*/
			virtual Result sendTraceInfo(const String& spanId, const String& opname, const SpanTagsType& tags) = 0;

		public:
			/** 设置全局关键日志信息回调
			*/
			class ITraceListener {
			public:
				virtual void onTraceLog(const String& info) = 0;
			protected:
				virtual ~ITraceListener() {
				}
			};
		public:
			virtual Result setTraceListener(ITraceListener* loglistenner) = 0;

		protected:
			virtual ~IAVDTrace() {
			}
		};

	} // namespace avd
} // namespace tee3

#endif//RT_AVDEngine_H_
