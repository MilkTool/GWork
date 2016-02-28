/*
 *  Gwork
 *  Copyright (c) 2010 Facepunch Studios
 *  Copyright (c) 2013-16 Billy Quith
 *  See license in Gwork.h
 */


#include "Gwork/Gwork.h"
#include "Gwork/Controls/Canvas.h"
#include "Gwork/Skin.h"
#include "Gwork/Controls/Menu.h"
#include "Gwork/DragAndDrop.h"
#include "Gwork/ToolTip.h"

#ifndef GWK_NO_ANIMATION
#include "Gwork/Anim.h"
#endif

using namespace Gwk::Controls;


Canvas::Canvas(Gwk::Skin::Base* pSkin) : ParentClass(NULL), m_bAnyDelete(false)
{
    SetBounds(0, 0, 10000, 10000);
    SetScale(1.0f);
    SetBackgroundColor(Color(255, 255, 255, 255));
    SetDrawBackground(false);

    if (pSkin)
        SetSkin(pSkin);
}

Canvas::~Canvas()
{
    ReleaseChildren();
}

void Canvas::RenderCanvas()
{
    DoThink();
    Gwk::Renderer::Base* render = m_Skin->GetRender();
    render->Begin();
    RecurseLayout(m_Skin);
    render->SetClipRegion(GetBounds());
    render->SetRenderOffset(Gwk::Point(0, 0));
    render->SetScale(Scale());

    if (m_bDrawBackground)
    {
        render->SetDrawColor(m_BackgroundColor);
        render->DrawFilledRect(GetRenderBounds());
    }

    DoRender(m_Skin);
    DragAndDrop::RenderOverlay(this, m_Skin);
    ToolTip::RenderToolTip(m_Skin);
    render->End();
}

void Canvas::Render(Gwk::Skin::Base* /*pRender*/)
{
    m_bNeedsRedraw = false;
}

void Canvas::OnBoundsChanged(Gwk::Rect oldBounds)
{
    ParentClass::OnBoundsChanged(oldBounds);
    InvalidateChildren(true);
}

void Canvas::DoThink()
{
    ProcessDelayedDeletes();

    if (Hidden())
        return;

#ifndef GWK_NO_ANIMATION
    Gwk::Anim::Think();
#endif
    // Reset tabbing
    {
        NextTab = NULL;
        FirstTab = NULL;
    }
    ProcessDelayedDeletes();
    // Check has focus etc..
    RecurseLayout(m_Skin);

    // If we didn't have a next tab, cycle to the start.
    if (NextTab == NULL)
        NextTab = FirstTab;

    Gwk::Input::OnCanvasThink(this);
}

void Canvas::SetScale(float f)
{
    if (m_fScale == f)
        return;

    m_fScale = f;

    if (m_Skin && m_Skin->GetRender())
        m_Skin->GetRender()->SetScale(m_fScale);

    OnScaleChanged();
    Redraw();
}

void Canvas::AddDelayedDelete(Gwk::Controls::Base* pControl)
{
    if (!m_bAnyDelete || m_DeleteSet.find(pControl) == m_DeleteSet.end())
    {
        m_bAnyDelete = true;
        m_DeleteSet.insert(pControl);
        m_DeleteList.push_back(pControl);
    }
}

void Canvas::PreDeleteCanvas(Gwk::Controls::Base* pControl)
{
    if (m_bAnyDelete)
    {
        std::set<Controls::Base*>::iterator itFind;

        if ((itFind = m_DeleteSet.find(pControl)) != m_DeleteSet.end())
        {
            m_DeleteList.remove(pControl);
            m_DeleteSet.erase(pControl);
            m_bAnyDelete = !m_DeleteSet.empty();
        }
    }
}

void Canvas::ProcessDelayedDeletes()
{
    while (m_bAnyDelete)
    {
        m_bAnyDelete = false;
        Controls::Base::List deleteList = m_DeleteList;
        m_DeleteList.clear();
        m_DeleteSet.clear();

        for (Gwk::Controls::Base::List::iterator it = deleteList.begin();
             it != deleteList.end();
             ++it)
        {
            Gwk::Controls::Base* pControl = *it;
            pControl->PreDelete(GetSkin());
            delete pControl;
            Redraw();
        }
    }
}

void Canvas::ReleaseChildren()
{
    Base::List::iterator iter = Children.begin();

    while (iter != Children.end())
    {
        Base* pChild = *iter;
        iter = Children.erase(iter);
        delete pChild;
    }
}

bool Canvas::InputMouseMoved(int x, int y, int deltaX, int deltaY)
{
    if (Hidden())
        return false;

    if (ToolTip::TooltipActive())
        Redraw();

    // Todo: Handle scaling here..
    // float fScale = 1.0f / Scale();
    Gwk::Input::OnMouseMoved(this, x, y, deltaX, deltaY);

    if (!Gwk::HoveredControl)
        return false;

    if (Gwk::HoveredControl == this)
        return false;

    if (Gwk::HoveredControl->GetCanvas() != this)
        return false;

    Gwk::HoveredControl->OnMouseMoved(x, y, deltaX, deltaY);
    Gwk::HoveredControl->UpdateCursor();
    DragAndDrop::OnMouseMoved(Gwk::HoveredControl, x, y);
    return true;
}

bool Canvas::InputMouseButton(int iButton, bool bDown)
{
    if (Hidden())
        return false;

    return Gwk::Input::OnMouseClicked(this, iButton, bDown);
}

bool Canvas::InputKey(int iKey, bool bDown)
{
    if (Hidden())
        return false;

    if (iKey <= Gwk::Key::Invalid)
        return false;

    if (iKey >= Gwk::Key::Count)
        return false;

    return Gwk::Input::OnKeyEvent(this, iKey, bDown);
}

bool Canvas::InputCharacter(Gwk::UnicodeChar chr)
{
    if (Hidden())
        return false;

    if (!iswprint(chr))
        return false;

    // Handle Accelerators
    if (Gwk::Input::HandleAccelerator(this, chr))
        return true;

    // Handle characters
    if (!Gwk::KeyboardFocus)
        return false;

    if (Gwk::KeyboardFocus->GetCanvas() != this)
        return false;

    if (!Gwk::KeyboardFocus->Visible())
        return false;

    if (Gwk::Input::IsControlDown())
        return false;

    return KeyboardFocus->OnChar(chr);
}

bool Canvas::InputMouseWheel(int val)
{
    if (Hidden())
        return false;

    if (!Gwk::HoveredControl)
        return false;

    if (Gwk::HoveredControl == this)
        return false;

    if (Gwk::HoveredControl->GetCanvas() != this)
        return false;

    return Gwk::HoveredControl->OnMouseWheeled(val);
}