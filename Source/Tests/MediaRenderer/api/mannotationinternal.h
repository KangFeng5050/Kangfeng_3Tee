#ifndef RT_AnnotationInternal_H_
#define RT_AnnotationInternal_H_

#include "common.h"
#include "room.h"
#include "annotationobject.h"

namespace tee3 {
	namespace avd {

		class RT_API_EXPORT IMAnnotationInternal {
		public:
			class IListener {
			public:
				virtual void onAnnotationAddNotify(const BoardId bid, AnnotationObject* annotationMsg) = 0;
				virtual void onAnnotationDelNotify(const BoardId bid, const AnnotationId& annoId) = 0;
				virtual void onAnnotationUpdateNotify(const BoardId bid, const AnnotationId& annoId, AnnotationObject* annobj) = 0;
				virtual void onAnnotationAddNotifyStr(const BoardId bid, const std::string& annotaionStr) = 0;
				virtual void onAnnotationUpdateNotifyStr(const BoardId bid, const AnnotationId& annoId, const std::string& annotaionStr) = 0;
				virtual void onAnnotationClearNotify(const BoardId bid,const std::string userid)= 0;
			protected:
				virtual ~IListener() {}
			};

		public:
			static IMAnnotationInternal* getAnnotationInternal(IRoom* room);
			static bool isSupported(AnnotationToolType type);
			virtual Result setListener(IMAnnotationInternal::IListener* listener) = 0;
			virtual Result addAnnotationObj(BoardId bid, AnnotationObject* annobj) = 0;
			virtual Result removeAnnotationObj(BoardId bid, const AnnotationId& id) = 0;
			virtual Result updateAnnotationObj(BoardId bid, const AnnotationId& id, AnnotationObject* annobj) = 0;
			//
			virtual Result addAnnotationStr(BoardId bid, const std::string& annostr) = 0;
			virtual Result updateAnnotationStr(BoardId bid, const AnnotationId& id, const std::string& annostr) = 0;
			virtual Result getAnnotationList(BoardId bid, AnnotationTypes& annos) = 0;
			virtual Result setDisplayRect(BoardId bid, uint32 left, uint32 top, uint32 right, uint32 bottom) = 0;

			///撤销,重做,清除
			virtual Result undo(BoardId bid) = 0;
		    virtual Result redo(BoardId bid) = 0;
			virtual Result clear(BoardId bid) = 0;

			virtual void redrawAll(BoardId bid)=0;
		public:
			virtual ~IMAnnotationInternal() {}
		};

	}
}
#endif//RT_AnnotationInternal_H_
