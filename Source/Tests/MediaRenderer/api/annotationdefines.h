#ifndef RT_AnnotationDefines_H_
#define RT_AnnotationDefines_H_

#include "common.h"
#include "annotationbasedefines.h"

NAMESPACE_TEE3_AVD_BEGIN

#ifndef LOG_FONT_FACE_NAME_SIZE
#define LOG_FONT_FACE_NAME_SIZE 32
#endif

#ifndef DEFAULT_LINE_WIDTH
#define DEFAULT_LINE_WIDTH 1
#endif
#ifndef DEFAULT_LINE_COLOR
#define DEFAULT_LINE_COLOR 0xFF000000
#endif
#ifndef DEFAULT_BGROUNG_COLOR
#define DEFAULT_BGROUNG_COLOR 0xFF0000FF
#endif
#ifndef DEFAULT_FILL_COLOR
#define DEFAULT_FILL_COLOR 0xFF000000
#endif
	
	typedef struct tagLOGFONTA {
		long      lfHeight;
		long      lfWidth;
		long      lfEscapement;
		long      lfOrientation;
		long      lfWeight;
		char      lfItalic;
		char      lfUnderline;
		char      lfStrikeOut;
		char      lfCharSet;
		char      lfOutPrecision;
		char      lfClipPrecision;
		char      lfQuality;
		char      lfPitchAndFamily;
		char      lfFaceName[LOG_FONT_FACE_NAME_SIZE];
	}LogFont;

static bool CalculateCheckMarkPoints(Point& pt1, Point& pt2, Point aPoints[]) {
	aPoints[0].x = pt1.x;
	aPoints[0].y = pt1.y + (pt2.y - pt1.y)*2.0 / 3.0;
	aPoints[1].x = (pt1.x + pt2.x) / 2;
	aPoints[1].y = pt2.y;
	aPoints[2].x = pt2.x;
	aPoints[2].y = pt1.y;
	return true;
}
using tee3::avd::WindowId;
using tee3::avd::DesktopId;
using tee3::avd::LineArrowType;
using tee3::avd::line_arrow_none;
class AVDContext {
public:
	AVDContext()
		: line_arrow_(line_arrow_none)
		, line_width_(DEFAULT_LINE_WIDTH)
		, line_color_(DEFAULT_LINE_COLOR)
		, fill_color_(DEFAULT_FILL_COLOR) {}
    virtual ~AVDContext(){}
	virtual void setDrawWindowId(const WindowId& id) { draw_window_id_ = id; }
	virtual void setDisplayWindowId(const WindowId& id) { display_window_id_ = id; }
	virtual void setLineWidth(int width) { line_width_ = width; }
	virtual void setLineColor(const Color& color) { line_color_ = color; }
	virtual void setFillColor(const Color& color) { fill_color_ = color; }
	virtual void setLineArrow(const LineArrowType& arrow) { line_arrow_ = arrow; }
	virtual void setDisplayRect(const Rect& rc) { display_rect_ = rc; }
	virtual void setAnnotationRect(const Rect& rc) { annotation_rect_ = rc; }
	virtual void setZoomMode(bool isZoom) { zoom_ = isZoom; }
	LineArrowType	lineArrow()  const { return line_arrow_; }
	int				lineWidth()  const { return line_width_; }
	int				displayWidth() const {
		int width = 0;
		if (0 != annotation_rect_.width()) {
			width = line_width_ * (double)display_rect_.width() / annotation_rect_.width() + 0.5;
		}
		else if (0 != annotation_rect_.height()) {
			width = line_width_ * (double)display_rect_.height() / annotation_rect_.height() + 0.5;
		}
		if (0 == width) {
			width = 1;
		}
		return width;
	}
	WindowId		dispayWindowId()  const { return display_window_id_; }
	WindowId		drawWindowId()  const { return draw_window_id_; }
	Color			lineColor()  const { return line_color_; }
	Color			fillColor()  const { return fill_color_; }
	Rect			displayRect() const { return display_rect_; }
	Rect			annotationRect() const { return annotation_rect_; }
	bool			zoomMode()const { return zoom_; }
	void clear() {
		line_arrow_ = line_arrow_none;
		line_width_ = DEFAULT_LINE_WIDTH;
		line_color_ = DEFAULT_LINE_COLOR;
		fill_color_ = DEFAULT_FILL_COLOR;
		display_window_id_ = WindowId::Cast(0);
		draw_window_id_ = WindowId::Cast(0);
		display_rect_ = Rect();
		annotation_rect_ = Rect();
	}


protected:
	LineArrowType		line_arrow_;
	int					line_width_;
	//not need initialize params
	WindowId			display_window_id_;
	WindowId			draw_window_id_;
	Color				line_color_;
	Color				fill_color_;
	Rect				display_rect_;
	Rect				annotation_rect_;
	bool				zoom_;
};

/**
* 堆数组模板类.
*/
template<class ItemType, int GrowSize>
class EMArrayBaseT2 {
public:
	EMArrayBaseT2() {
		m_head = NULL;
		m_allocSize = 0;
		m_usedSize = 0;
	}

	EMArrayBaseT2(const EMArrayBaseT2& other) {
		m_head = NULL;
		m_allocSize = 0;
		m_usedSize = 0;

		*this = other;
	}

	virtual ~EMArrayBaseT2() {
		RemoveAll();
	}

public:
	int GetCount() {
		return m_usedSize;
	}

	bool Add(ItemType item, bool bCheckDuplicate = false) {
		if (Expand()) {
			if (bCheckDuplicate && GetIndex(item) >= 0)
				return false;

			m_head[m_usedSize++] = item;
			return true;
		}
		return false;
	}

	bool InsertAt(int nIndex, ItemType item) {
		if (nIndex >= 0 && nIndex <= m_usedSize) {
			if (!Expand())
				return false;

			if (nIndex < m_usedSize)
				memmove(&m_head[nIndex + 1], &m_head[nIndex],
					sizeof(ItemType)*(m_usedSize - nIndex));

			m_head[nIndex] = item;
			m_usedSize++;

			return true;
		}
		return false;
	}

	bool Remove(ItemType item) {
		int i = 0;
		for (; i < m_usedSize; i++) {
			if (Compare(m_head + i, &item)) {
				break;
			}
		}

		if (i < m_usedSize) {
			// found
			if (i < m_usedSize - 1)
				memcpy(&m_head[i], &m_head[i + 1],
					sizeof(ItemType)*(m_usedSize - i - 1));
			m_usedSize--;
			return  true;
		}

		return false;
	}

	bool RemoveAt(int i) {
		if (i < m_usedSize) {
			if (i < m_usedSize - 1)
				memcpy(&m_head[i], &m_head[i + 1],
					sizeof(ItemType)*(m_usedSize - i - 1));
			m_usedSize--;
			return  true;
		}
		return false;
	}

	void RemoveAll() {
		if (m_head)
			delete[]m_head;
		m_head = NULL;
		m_allocSize = 0;
		m_usedSize = 0;
	}

	EMArrayBaseT2& operator = (const EMArrayBaseT2& other) {
		RemoveAll();

		if (other.m_usedSize > 0) {
			m_head = new ItemType[other.m_usedSize];
			if (m_head) {
				memcpy(m_head, other.m_head, sizeof(ItemType)*other.m_usedSize);
				m_allocSize = m_usedSize = other.m_usedSize;
			}
		}

		return *this;
	}

	ItemType operator [](int i) {
		RT_ASSERTE(m_head != NULL);
		ItemType retVal;
		if (i >= 0 && i < m_usedSize) {
			retVal = m_head[i];
		}
		return retVal;
	}

	bool SetAt(int nIndex, ItemType item) {
		RT_ASSERTE(nIndex >= 0 && nIndex < m_usedSize);

		if (nIndex >= 0 && nIndex < m_usedSize) {
			m_head[nIndex] = item;

			return true;
		}

		return false;
	}

	int GetIndex(ItemType item) {
		for (int i = 0; i < m_usedSize; i++) {
			if (Compare(m_head + i, &item))
				return	i;
		}

		return -1;
	}

	ItemType* GetHead() {
		return m_head;
	}

protected:
	bool Compare(ItemType* item1, ItemType* item2) {
		if (memcmp(item1, item2, sizeof(ItemType)) == 0)
			return	true;

		return	false;
	}

	bool Expand() {
		if (m_usedSize >= m_allocSize) {
			ItemType* ppNew = new ItemType[m_allocSize + GrowSize];
			if (ppNew) {
				if (m_head) {
					memcpy(ppNew, m_head, sizeof(ItemType)*m_allocSize);
					delete[]m_head;
				}
				m_head = ppNew;
				m_allocSize += GrowSize;
				return true;
			}
			return false;
		}
		return true;
	}

protected:
	ItemType*	m_head;
	int			m_allocSize;
	int			m_usedSize;
};
template<class ItemType, int GrowSize>
class EMArrayBaseT {
public:
	EMArrayBaseT() {
		m_ppHead = NULL;
		m_nAlloc = 0;
		m_nUsed = 0;
	}

	EMArrayBaseT(const EMArrayBaseT& other) {
		m_ppHead = NULL;
		m_nAlloc = 0;
		m_nUsed = 0;

		*this = other;
	}

	virtual ~EMArrayBaseT() {
		RemoveAll();
	}

public:
	int GetCount() {
		return m_nUsed;
	}

	bool Add(ItemType item, bool bCheckDuplicate = false) {
		if (Expand()) {
			if (bCheckDuplicate && GetIndex(item) >= 0)
				return false;

			m_ppHead[m_nUsed++] = item;
			return true;
		}
		return false;
	}

	bool InsertAt(int nIndex, ItemType item, bool bCheckDuplicate = false) {
		if (bCheckDuplicate && GetIndex(item) >= 0)
			return false;

		if (nIndex >= 0 && nIndex <= m_nUsed) {
			if (!Expand())
				return false;

			if (nIndex < m_nUsed)
				memmove(&m_ppHead[nIndex + 1], &m_ppHead[nIndex],
					sizeof(ItemType)*(m_nUsed - nIndex));

			m_ppHead[nIndex] = item;
			m_nUsed++;

			return true;
		}
		return false;
	}

	bool Remove(ItemType item) {
		int i = 0;
		for (i = 0; i < m_nUsed; i++) {
			if (Compare(m_ppHead + i, &item)) {
				break;
			}
		}

		if (i < m_nUsed) {
			// found
			if (i < m_nUsed - 1)
				memcpy(&m_ppHead[i], &m_ppHead[i + 1],
					sizeof(ItemType)*(m_nUsed - i - 1));
			m_nUsed--;
			return  true;
		}

		return false;
	}

	bool RemoveAt(int i) {
		if (i < m_nUsed) {
			if (i < m_nUsed - 1)
				memcpy(&m_ppHead[i], &m_ppHead[i + 1],
					sizeof(ItemType)*(m_nUsed - i - 1));
			m_nUsed--;
			return  true;
		}
		return false;
	}

	void RemoveAll() {
		if (m_ppHead)
			delete[]m_ppHead;
		m_ppHead = NULL;
		m_nAlloc = 0;
		m_nUsed = 0;
	}

	EMArrayBaseT& operator = (const EMArrayBaseT& other) {
		RemoveAll();

		if (other.m_nUsed > 0) {
			m_ppHead = new ItemType[other.m_nUsed];
			if (m_ppHead) {
				memcpy(m_ppHead, other.m_ppHead, sizeof(ItemType)*other.m_nUsed);
				m_nAlloc = m_nUsed = other.m_nUsed;
			}
		}

		return *this;
	}

	ItemType operator [](int i) {
		ItemType retVal = NULL;

		if (i >= 0 && i < m_nUsed) {
			retVal = m_ppHead[i];
		}

		return retVal;
	}

	bool SetAt(int nIndex, ItemType item) {
		if (nIndex >= 0 && nIndex < m_nUsed) {
			m_ppHead[nIndex] = item;

			return true;
		}

		return false;
	}

	int GetIndex(ItemType item) {
		switch (sizeof(ItemType)) {
		case	4:
		{
			int dwItem = (int)item;
			for (int i = 0; i < m_nUsed; i++) {
				if (((int)m_ppHead[i]) == dwItem)
					return	i;
			}
		}
		break;
		case	2:
		{
			short wItem = (short)item;
			for (int i = 0; i < m_nUsed; i++) {
				if (((short)m_ppHead[i]) == wItem)
					return	i;
			}
		}
		break;
		case	1:
		{
			char cbItem = (char)item;
			for (int i = 0; i < m_nUsed; i++) {
				if (((char)m_ppHead[i]) == cbItem)
					return	i;
			}
		}
		break;
		default:
			for (int i = 0; i < m_nUsed; i++) {
				if (Compare(m_ppHead + i, &item))
					return	i;
			}
			break;
		}

		return -1;
	}

	ItemType* GetHead() {
		return m_ppHead;
	}

protected:
	bool Compare(ItemType* item1, ItemType* item2) {
		if (memcmp(item1, item2, sizeof(ItemType)) == 0)
			return	true;

		return	false;
	}

	bool Expand() {
		if (m_nUsed >= m_nAlloc) {
			ItemType* ppNew = new ItemType[m_nAlloc + GrowSize];
			if (ppNew) {
				memset(ppNew, 0, (m_nAlloc + GrowSize) * sizeof(ItemType));

				if (m_ppHead) {
					memcpy(ppNew, m_ppHead, sizeof(ItemType)*m_nAlloc);
					delete[]m_ppHead;
				}
				m_ppHead = ppNew;
				m_nAlloc += GrowSize;
				return true;
			}
			return false;
		}
		return true;
	}

protected:
	ItemType* m_ppHead;
	int     m_nAlloc;
	int     m_nUsed;
};
template <class ItemType>
class EMDynamicBuffer {
public:
	EMDynamicBuffer(uint32 nSize = 1024) {
		m_pHead = NULL;
		m_nSize = 0;
		SetSize(nSize);
	}

	~EMDynamicBuffer() {
		Reset();
	}

	void Reset() {
		if (m_pHead)
			delete[]m_pHead;
		m_pHead = NULL;
		m_nSize = 0;
	}

	int GetSize() {
		return m_nSize;
	}

	bool SetSize(uint32 nSize) {
		if (nSize > m_nSize) {
			ItemType* pNewHead = new ItemType[nSize];
			if (pNewHead) {
				memset(pNewHead, 0, sizeof(ItemType)*nSize);
				if (m_pHead) {
					memcpy(pNewHead, m_pHead, sizeof(ItemType)*m_nSize);
					delete[]m_pHead;
				}

				m_pHead = pNewHead;
				m_nSize = nSize;
				return true;
			}
			return false;
		}
		return true;
	}

	ItemType* GetHead() {
		return m_pHead;
	}
	operator ItemType* () { return m_pHead; }

protected:
	ItemType* m_pHead;
	uint32 m_nSize;
};
NAMESPACE_TEE3_AVD_END
#endif //RT_AnnotationDefines_H_
