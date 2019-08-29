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
	bool mCaseSensitive;//�Ƿ����ִ�Сд
	bool mFindBackward;//���»����ϲ���
	bool mWholeWord;//�Ƿ�ȫ��ƥ��
	QString mKeyword;
};
