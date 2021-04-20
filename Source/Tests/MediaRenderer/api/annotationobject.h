#ifndef RT_AnnotationObject_H_
#define RT_AnnotationObject_H_

#include "api/common.h"
#include "api/annotationdefines.h"


using tee3::avd::AnnotationToolType;
namespace rtc {
	class CriticalSection;
}
namespace room_pdu {
	class AnnotationObject;
	class Annotation2Point;
	class AnnotationMultiPoint;
	class AnnotationList;
}

NAMESPACE_TEE3_AVD_BEGIN

class RT_API_EXPORT AnnotationObject {
public:
	static AnnotationObject* Create(AnnotationToolType type);
	static AnnotationToolType checkToolType(std::string json);
	virtual ~AnnotationObject() {}

		//object info
		virtual void SetLineWidth(int width) { line_width_ = width; }
		virtual void SetLineColor(Color color) { line_color_ = color; }
		virtual void SetFillColor(Color color) { fill_color_ = color; }
		virtual void SetLineArrow(const LineArrowType& arrow) { line_arrow_ = arrow; }
		virtual void SetEnable(bool enable = true) { enable_ = enable; }
		virtual void SetDisplayRect(const Rect& rc) { display_rect_ = rc; }
		virtual void SetAnnotationRect(const Rect& rc) { annotation_rect_ = rc; }
		virtual void SetZoomMode(bool isZoom) { zoom_ = isZoom; }
		virtual bool IsEnable() { return enable_; }
		virtual bool Is(AnnotationToolType type) { return tool_type_ == type; };
		virtual AnnotationToolType ToolType() { return tool_type_; }

		virtual bool Draw(const AVDContext& context, bool paint) { return true; }
		virtual void CopyDataFrom(AnnotationObject* data) {}


		void CopyDataToPdu(room_pdu::AnnotationObject* pduobject);
		void CopyDataFromPdu(const room_pdu::AnnotationObject* pduobject);

	//json
	virtual bool CopyDataFromJson(std::string annotStr);
	virtual bool CopyDataToJson(std::string& annotStr);

		virtual bool HitTest(const tee3::avd::Point& pt) { return true; }

	//id info
	void SetUserId(const std::string& id) { user_id_ = id; }
	void SetAnnotationId(const std::string& id) { annotation_id_ = id; }
	void SetObjectId(const std::string& id) { object_id_ = id; }

	std::string GetUserId() { return user_id_; }
	std::string GetAnnotationId() { return annotation_id_; }
	std::string GetObjectId() { return object_id_; }
	tee3::avd::Color		GetLineColor() { return line_color_; }
	void setSeqNum(uint32 i) { seq_num_ = i; }
	uint32 getSeqNum() { return seq_num_; }
protected:
	AnnotationObject();
	AnnotationToolType	tool_type_;
	LineArrowType		line_arrow_;
	bool				owned_cursor_;
	bool				touched_;
	bool				enable_;
	bool				zoom_;
	int					line_width_;

	//not need initialize params
	Color				line_color_;
	Color				fill_color_;
	Rect				object_rect_;
	Rect				display_rect_;
	Rect				annotation_rect_;

	//user info
	std::string			object_id_;
	std::string			annotation_id_;
	std::string			user_id_;
	uint32              seq_num_;
};

typedef std::vector <AnnotationObject* > AnnotationObjects;
typedef std::vector <std::string > AnnotationObjectIds;
class Annotation2Point : public AnnotationObject {
public:
	void CopyDataFrom(AnnotationObject* data) override;
	void CopyDataFrom(const room_pdu::Annotation2Point* pdu2point) ;
	void CopyDataFromEx(const room_pdu::Annotation2Point* pdu2point) ;
	void CopyDataTo(room_pdu::Annotation2Point* pdu2point) ;
	void SetDisplayRect(const Rect& rc) override;
	bool CopyDataFromJson(std::string annotStr) override;
	bool CopyDataToJson(std::string& annotStr) override;
public:
		tee3::avd::Point	point_from_;
		tee3::avd::Point	point_to_;
};

class AnnotationLine : public Annotation2Point {
public:
		bool HitTest(const tee3::avd::Point& pt) override;
		AnnotationLine();
	};
	class AnnotationRectangle : public Annotation2Point {
	public:
		AnnotationRectangle();
		bool HitTest(const tee3::avd::Point& pt) override;
	protected:
	};
	class AnnotationEllipse : public Annotation2Point {
	public:
		AnnotationEllipse();
		bool HitTest(const tee3::avd::Point& pt) override;
	protected:
	};
	class AnnotationRhomb : public Annotation2Point {
	public:
		AnnotationRhomb();
		bool HitTest(const tee3::avd::Point& pt) override;
	protected:
	};

	class AnnotationArrow : public Annotation2Point {
	public:
		AnnotationArrow();
		bool HitTest(const tee3::avd::Point& pt) override;
	protected:
	};

	class AnnotationSuccess : public Annotation2Point {
	public:
		AnnotationSuccess();
		bool HitTest(const tee3::avd::Point& pt) override;
	protected:
	};

	class AnnotationFailure : public Annotation2Point {
	public:
		AnnotationFailure();
		bool HitTest(const tee3::avd::Point& pt) override;
	protected:
	};

	class AnnotationMultiPoint : public AnnotationObject {
	public:
		void CopyDataFrom(AnnotationObject* data) override;
		void CopyDataFrom(const room_pdu::AnnotationMultiPoint* pdupoints);
		void CopyDataFromEx(const room_pdu::AnnotationMultiPoint* pdupoints);
		void CopyDataTo(room_pdu::AnnotationMultiPoint* pdupoints);
		void SetDisplayRect(const Rect& rc) override;
		bool CopyDataFromJson(std::string annotStr) override;
		bool CopyDataToJson(std::string& annotStr) override;
	protected:
		PointArray	points_;
	};

	class AnnotationPolyline : public AnnotationMultiPoint {
	public:
		AnnotationPolyline();
		virtual bool HitTest(const tee3::avd::Point& pt) override;
	protected:
	};


	class AnnotationEraser : public AnnotationObject {
	public:
		using AnnotationObject::CopyDataFrom;
		AnnotationEraser(AnnotationObjects& eraserObjects);
		AnnotationEraser(AnnotationObjectIds& eraserObjectIds);
		AnnotationEraser();
		~AnnotationEraser();
		void Undo();
		void Redo();
		void CopyDataFrom(const room_pdu::AnnotationList* pdulist);
		void CopyDataTo(room_pdu::AnnotationList* pdulist);
		void SetEraserObjects(AnnotationObjects& eraserObjects);
		void GetEraserObjectIds(AnnotationObjectIds& eraserObjectIds);
		bool HitTest(const tee3::avd::Point& pt) override { return false; }
		bool Draw(const AVDContext& context, bool paint) override { return false; }
		void MoveOnly(const tee3::avd::Point& movePt) {};
	protected:
		AnnotationObjects eraser_objects_;
		AnnotationObjectIds eraser_object_ids_;
		rtc::CriticalSection*	map_crit_;
	};

	class AnnotationHighlightPoint : public Annotation2Point {
	public:
		AnnotationHighlightPoint();
		virtual bool HitTest(const tee3::avd::Point& pt) override { return false; }
		virtual bool Draw(const AVDContext& context, bool paint) override { return false; }
	protected:
	};
NAMESPACE_TEE3_AVD_END
#endif//RT_AnnotationObject_H_
