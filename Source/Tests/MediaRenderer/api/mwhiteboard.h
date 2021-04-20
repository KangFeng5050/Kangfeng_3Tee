#ifndef RT_WhiteBoard_H_
#define RT_WhiteBoard_H_

#include "common.h"
#include "room.h"
#include "annotationdefines.h"

namespace tee3 {
	namespace avd {
		class IRoom;
		class IMAnnotationInternal;
		class IMAnnotation;
		/**
		* 白板信息结构.
		*/
		struct Whiteboard{
			String          boardId;
			String          ownId;///白板创建者
			String			title;
			String			description;
			String          filenamebkg;///背景图地址
			uint32          width;
			uint32          height;
			uint32          displayheight;
			uint32          displaywidth;
			tee3::avd::Color   clr;
			WindowId        wndid; //应用层自建窗口白板句柄，SDK在该窗口上绘制
			WindowId        parentid; //SDK创建白板窗口，且指定父窗口句柄
			bool			toolbar; //toolbar设置为false 创建白板时将不创建ToolBar，需要应用成自定义Toolbar        
			Whiteboard() :width(0), height(0), displayheight(0), displaywidth(0), toolbar(true){}
		};
		/**
		* 白板集合类型.
		*/
		typedef std::vector<Whiteboard> WhiteboardsType;

		class RT_API_EXPORT IMWhiteboard : public IModule{
		public:
			class IListener {
			public:
				virtual void onCreateBoardNotify(const Whiteboard board) = 0;
				virtual void onRemoveBoardNotify(const BoardId bid) = 0;
				virtual void onUpdateBoardNotify(const Whiteboard board) = 0;
				virtual void onShareBoardNotify(const Whiteboard board) = 0;
				virtual void onCloseBoardNotify(const BoardId bid) = 0;
				virtual void onSaveBmpNotify(Result ret,std::string msg) = 0;

			protected:
				virtual ~IListener() {}
			};
			/** 获取白板模块接口
			*
			* @param[in] room 房间对象指针。
			*
			* @return 返回白板模块接口指针。
			*/
			static IMWhiteboard* getWhiteboard(IRoom* room);

			virtual Result setListener(IListener* listener) = 0;
			virtual Result createBoard(const Whiteboard& wb) = 0;
			virtual Result shareBoard(const BoardId id) = 0;
			virtual Result removeBoard(BoardId id) = 0;
			virtual Result closeBoard(BoardId id) = 0;
			virtual Result updateBoard(BoardId id, const Whiteboard& wb) = 0;
			virtual Result getWhiteBoards(WhiteboardsType& whbs) = 0;
			virtual Result getSharedWhiteBoards(WhiteboardsType& whbs) = 0;
			virtual Result setBackground(BoardId id, String filePath) = 0;
			virtual Result setBackground(BoardId id, uint8 r, uint8 g, uint8 b, uint8 a) = 0;
			virtual Result setWidth(BoardId id, uint32 w) = 0;
			virtual Result setHeight(BoardId id, uint32 h) = 0;
			virtual Result attachBoard(const BoardId& bid, WindowId winId) = 0;
			virtual Result detachBoard(const BoardId& bid) = 0;
			virtual IMAnnotationInternal* getAnnotationInternal() = 0;
			virtual IMAnnotation* getAnnotation(const BoardId& bid) = 0;
		protected:
			virtual ~IMWhiteboard() {}
		};
	}
}
#endif//RT_WhiteBoard_H_
