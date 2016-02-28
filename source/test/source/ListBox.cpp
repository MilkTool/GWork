/*
 *  Gwork
 *  Copyright (c) 2010 Facepunch Studios
 *  Copyright (c) 2013-16 Billy Quith
 *  See license in Gwork.h
 */

#include "Gwork/Test/Test.h"
#include "Gwork/Controls/ListBox.h"

using namespace Gwk;

class ListBox : public GUnit
{
public:

    GWK_CONTROL_INLINE(ListBox, GUnit)
    {
        {
            Gwk::Controls::ListBox* ctrl = new Gwk::Controls::ListBox(this);
            ctrl->SetBounds(10, 10, 100, 200);
            ctrl->AddItem("First");
            ctrl->AddItem("Blue");
            ctrl->AddItem("Yellow");
            ctrl->AddItem("Orange");
            ctrl->AddItem("Brown");
            ctrl->AddItem("Black");
            ctrl->AddItem("Green");
            ctrl->AddItem("Dog");
            ctrl->AddItem("Cat Blue");
            ctrl->AddItem("Shoes");
            ctrl->AddItem("Shirts");
            ctrl->AddItem("Chair");
            ctrl->AddItem("Last");
            ctrl->SelectByString("Bl*", true);
            ctrl->SetAllowMultiSelect(true);
            ctrl->SetKeyboardInputEnabled(true);
            ctrl->onRowSelected.Add(this, &ThisClass::RowSelected);
        }
        {
            Gwk::Controls::ListBox* ctrl = new Gwk::Controls::ListBox(this);
            ctrl->SetBounds(120, 10, 200, 200);
            ctrl->SetColumnCount(3);
            ctrl->SetAllowMultiSelect(true);
            ctrl->onRowSelected.Add(this, &ThisClass::RowSelected);
            {
                Gwk::Controls::Layout::TableRow* pRow = ctrl->AddItem("Baked Beans");
                pRow->SetCellText(1, "Heinz");
                pRow->SetCellText(2, "£3.50");
            }
            {
                Gwk::Controls::Layout::TableRow* pRow = ctrl->AddItem("Bananas");
                pRow->SetCellText(1, "Trees");
                pRow->SetCellText(2, "$1.27");
            }
            {
                Gwk::Controls::Layout::TableRow* pRow = ctrl->AddItem("Chicken");
                pRow->SetCellText(1, Gwk::Utility::Narrow(L"\u5355\u5143\u6D4B\u8BD5"));
                pRow->SetCellText(2, Gwk::Utility::Narrow(L"\u20AC8.95"));
            }
        }
    }


    void RowSelected(Gwk::Controls::Base* pControl)
    {
        Gwk::Controls::ListBox* ctrl = (Gwk::Controls::ListBox*)pControl;
        UnitPrint(Utility::Format("Listbox Item Selected: %s",
                                  ctrl->GetSelectedRow()->GetText(0).c_str()));
    }

    Gwk::Font m_Font;
};


DEFINE_UNIT_TEST(ListBox);
