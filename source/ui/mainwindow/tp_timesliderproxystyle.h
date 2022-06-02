#ifndef TP_TIMESLIDERPROXYSTYLE_H
#define TP_TIMESLIDERPROXYSTYLE_H

#include <QProxyStyle>

class TP_TimeSliderProxyStyle : public QProxyStyle
{
    Q_OBJECT

public:
    explicit TP_TimeSliderProxyStyle( QStyle *style );
    int styleHint( QStyle::StyleHint hint,
                   const QStyleOption *option = nullptr,
                   const QWidget *widget = nullptr,
                   QStyleHintReturn *returnData = nullptr ) const override;
};

#endif // TP_TIMESLIDERPROXYSTYLE_H
