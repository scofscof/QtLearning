#include "QFindDlg.h"

QFindDlg::QFindDlg(QWidget *parent)
	: QDialog(parent)
{
	ui.setupUi(this);
	ui.radioButtonDown->setChecked(true);
	mFindBackward = false;
	mCaseSensitive = false;
	mWholeWord = false;
	InitConnect();
}

QFindDlg::~QFindDlg()
{

}

const QString& QFindDlg::GetKeyword() const
{
	return mKeyword;
}

bool QFindDlg::IsCaseSensitive() const
{
	return mCaseSensitive;
}

bool QFindDlg::IsWholeWord() const
{
	return mWholeWord;
}

bool QFindDlg::isFindBackward() const
{
	return mFindBackward;
}

void QFindDlg::SetCaseSensitive(bool isCheck)
{
	mCaseSensitive = isCheck;
}

void QFindDlg::SetFindDirection()
{
	if (ui.radioButtonDown->isChecked())
	{ 
		mFindBackward = false;
	}
	else if (ui.radioButtonUp->isChecked())
	{
		mFindBackward = true;
	}
}

void QFindDlg::SetWholeWord(bool isCheck)
{
	mWholeWord = isCheck;
}

void QFindDlg::KeywordChanged(const QString& keyWord)
{
	mKeyword = keyWord;
}

void QFindDlg::InitConnect()
{
	connect(ui.checkBoxCaseSensitive, SIGNAL(toggled(bool)), this, SLOT(SetCaseSensitive(bool)));
	connect(ui.checkBoxWholeWord, SIGNAL(toggled(bool)), this, SLOT(SetWholeWord(bool)));
	connect(ui.radioButtonDown, SIGNAL(clicked()), this, SLOT(SetFindDirection()));
	connect(ui.radioButtonUp, SIGNAL(clicked()), this, SLOT(SetFindDirection()));
	connect(ui.lineEditContent, SIGNAL(textChanged(const QString&)), this, SLOT(KeywordChanged(const QString&)));
	connect(ui.pushButtonFindNext, SIGNAL(clicked()), parentWidget(), SLOT(FindNext()));
}
