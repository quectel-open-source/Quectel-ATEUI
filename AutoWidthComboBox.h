#ifndef AUTOWIDTHCOMBOBOX_H
#define AUTOWIDTHCOMBOBOX_H
#include <QApplication>
#include <QComboBox>
#include <QFontMetrics>
#include <QStringList>
#include <QDebug>
#include <QWidget>
#include <QVBoxLayout>
#include <QPushButton>
#include <QAbstractItemView>

class AutoWidthComboBox : public QComboBox
{
    Q_OBJECT
public:
    explicit AutoWidthComboBox(QWidget *parent = nullptr) : QComboBox(parent)
    {
        // 启用下拉框时自动调整宽度
        connect(this, QOverload<int>::of(&QComboBox::activated),
                this, &AutoWidthComboBox::adjustPopupWidth);
    }

protected:
    void showEvent(QShowEvent *e) override
    {
        QComboBox::showEvent(e);
        adjustPopupWidth();
    }

public slots:
    void adjustPopupWidth()
    {
        // 计算所有项目的最大宽度
        int maxWidth = 0;
        QFontMetrics fm(font());

        // 考虑下拉箭头和边距
        const int iconWidth = 20; // 图标宽度
        const int margin = 40;    // 边距

        // 遍历所有项目
        for (int i = 0; i < count(); ++i)
        {
            int itemWidth = fm.horizontalAdvance(itemText(i));
            if (itemWidth > maxWidth)
            {
                maxWidth = itemWidth;
            }
        }

        // 添加额外宽度以容纳图标和滚动条
        int totalWidth = maxWidth + iconWidth + margin;

        // 设置视图的最小宽度
        if (view())
        {
            view()->setMinimumWidth(totalWidth);
            qDebug() << "下拉框宽度调整为:" << totalWidth;
        }
    }
};

#endif // AUTOWIDTHCOMBOBOX_H
