#pragma once

namespace DemoParticles
{
    static const char* SequencerItemTypeNames[] = { "Camera","Music", "ScreenEffect", "FadeIn", "Animation" };


    struct RampEdit : public ImCurveEdit::Delegate
    {
        RampEdit();
        size_t GetCurveCount();

        bool IsVisible(size_t curveIndex);
        size_t GetPointCount(size_t curveIndex);

        uint32_t GetCurveColor(size_t curveIndex);
        ImVec2* GetPoints(size_t curveIndex);
        virtual ImCurveEdit::CurveType GetCurveType(size_t curveIndex) const;
        virtual int EditPoint(size_t curveIndex, int pointIndex, ImVec2 value);
        virtual void AddPoint(size_t curveIndex, ImVec2 value);
        virtual ImVec2& GetMax();
        virtual ImVec2& GetMin();
        virtual unsigned int GetBackgroundColor();
        ImVec2 mPts[3][8];
        size_t mPointCount[3];
        bool mbVisible[3];
        ImVec2 mMin;
        ImVec2 mMax;
    private:
        void SortValues(size_t curveIndex);
    };

    struct MySequence : public ImSequencer::SequenceInterface
    {
        // interface with sequencer

        virtual int GetFrameMin() const;
        virtual int GetFrameMax() const;
        virtual int GetItemCount() const;

        virtual int GetItemTypeCount() const;
        virtual const char *GetItemTypeName(int typeIndex) const;
        virtual const char *GetItemLabel(int index) const;

        virtual void Get(int index, int** start, int** end, int *type, unsigned int *color);
        virtual void Add(int type);;
        virtual void Del(int index);
        virtual void Duplicate(int index);

        virtual size_t GetCustomHeight(int index);

        // my datas
        MySequence();
        int mFrameMin, mFrameMax;
        struct MySequenceItem
        {
            int mType;
            int mFrameStart, mFrameEnd;
            bool mExpanded;
        };
        std::vector<MySequenceItem> myItems;
        RampEdit rampEdit;

        virtual void DoubleClick(int index);

        virtual void CustomDraw(int index, ImDrawList* draw_list, const ImRect& rc, const ImRect& legendRect, const ImRect& clippingRect, const ImRect& legendClippingRect);

        virtual void CustomDrawCompact(int index, ImDrawList* draw_list, const ImRect& rc, const ImRect& clippingRect);
    };
}