#include "tp_timesliderproxystyle.h"

TP_TimeSliderProxyStyle::TP_TimeSliderProxyStyle( QStyle *style ) :
    QProxyStyle { style }
{

}

int
TP_TimeSliderProxyStyle::styleHint( QStyle::StyleHint hint,
                                    const QStyleOption *option,
                                    const QWidget *widget,
                                    QStyleHintReturn *returnData ) const
{
    if (hint == QStyle::SH_Slider_AbsoluteSetButtons)
        return Qt::LeftButton;

    return QProxyStyle::styleHint(hint, option, widget, returnData);
}
