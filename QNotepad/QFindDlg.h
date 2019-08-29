#pragma once

#include <QDialog>
#include "ui_QFindDlg.h"

class QTextDocument;
class QFindDlg : public QDialog
{
	Q_OBJECT
public:
	QFindDlg(QWidget *parent = Q_NULLPTR);
	~QFindDlg();
	const QString& GetKeyword() const;
	bool IsCaseSensitive() const;
	bool IsWholeWord() const;
	bool isFindBackward() const;
private slots:
	void SetCaseSensitive(bool isCheck);
	void SetFindDirection();
	void SetWholeWord(bool isCheck);
	void KeywordChanged(const QString& keyWord);
private:
	void InitConnect();
private:
	Ui::QFindDlg ui;
	bool mCaseSensitive;//是否区分大小写
	bool mFindBackward;//向下或向上查找
	bool mWholeWord;//是否全字匹配
	QString mKeyword;
};
