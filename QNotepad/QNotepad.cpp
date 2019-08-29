#include "QNotepad.h"
#include<QFileDialog>
#include <QStandardPaths>
#include <QMessageBox>
#include <QDateTime>
#include <QTextDocument>

static const QString titleSuffix = "-记事本";
static const QString defaultTitle = "无标题";
QNotepad::QNotepad(QWidget *parent)
	: QMainWindow(parent)
	, mSettings("./Setting.ini", QSettings::IniFormat)
	,mFindDlg(this)
{
	ui.setupUi(this);
	setWindowTitle(defaultTitle + titleSuffix);
	mChanged = false;
	mCurrentCursor.setPosition(0);
	InitShortcut();
	InitConnect();
}

void QNotepad::UpdateFileStatus()
{
	QString text = ui.plainTextEdit->toPlainText();
	mChanged = (!text.isEmpty());
}

void QNotepad::NewFile()
{
	if (mChanged)
	{
		if (!PreCheck())
		{
			return;
		}
	}
	mChanged = false;
	ui.plainTextEdit->clear();
	mCurrentFilePath.clear();
	setWindowTitle(defaultTitle + titleSuffix);
}

void QNotepad::OpenFile()
{
	if (mChanged)
	{
		if (!PreCheck())
		{
			return;
		}
	}
	QString lastPath = mSettings.value("LastFilePath").toString();
	if (lastPath.isEmpty())
	{
		lastPath = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);
	}
	mCurrentFilePath = QFileDialog::getOpenFileName(this,
		tr("打开"),
		lastPath, 
		tr("文本文档(*.txt);;所有文件(*.*)"));
	if (mCurrentFilePath.isEmpty())
	{
		return;
	}
	QFile file(mCurrentFilePath);
	if (!file.open(QIODevice::Text | QIODevice::ReadOnly))
	{
		return;
	}
	QByteArray text = std::move(file.readAll());
	ui.plainTextEdit->setPlainText(QString(text));
	file.close();
	mChanged = false;
	QFileInfo fileInfo(mCurrentFilePath);
	setWindowTitle(fileInfo.baseName() + titleSuffix);
	mSettings.setValue("LastFilePath", fileInfo.absolutePath());
}

bool QNotepad::SaveFile()
{
	if (mCurrentFilePath.isEmpty())
	{
		QString lastPath = mSettings.value("LastFilePath").toString();
		if (lastPath.isEmpty())
		{
			lastPath = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);
		}
		mCurrentFilePath = QFileDialog::getSaveFileName(this,
			tr("另存为"),
			lastPath,
			tr("文本文档(*.txt);;所有文件(*.*)"));
		if (mCurrentFilePath.isEmpty())
		{
			return false;
		}
	}
	QFile file(mCurrentFilePath);
	if (!file.open(QIODevice::Text | QIODevice::WriteOnly))
	{
		SaveAsFile();
		return false;
	}
	file.write(ui.plainTextEdit->toPlainText().toStdString().c_str());
	file.close();
	mChanged = false;
	return true;
}

void QNotepad::SaveAsFile()
{
	QString oldFilePath = mCurrentFilePath;
	mCurrentFilePath.clear();
	if (SaveFile())
	{
		QFileInfo fileInfo(mCurrentFilePath);
		setWindowTitle(fileInfo.baseName() + titleSuffix);
		mSettings.setValue("LastFilePath", fileInfo.absolutePath());
	}
	else
	{
		mCurrentFilePath = oldFilePath;
	}
}

void QNotepad::InsertSysDataTime()
{
	QDateTime curDateTime = QDateTime::currentDateTime();
	QString strDateTime = curDateTime.toString(Qt::DateFormat::DefaultLocaleShortDate);
	ui.plainTextEdit->textCursor().insertText(strDateTime);
}

void QNotepad::Find()
{
	mFindDlg.show();
}

void QNotepad::FindNext()
{
	const QString& keyword = mFindDlg.GetKeyword();
	if (keyword.isEmpty())
	{
		return;
	}
	QTextDocument* doc = ui.plainTextEdit->document();
	QTextDocument::FindFlags options;
	if (mFindDlg.IsCaseSensitive())
	{
		options |= QTextDocument::FindCaseSensitively;
	}
	if (mFindDlg.isFindBackward())
	{
		options |= QTextDocument::FindBackward;
	}
	if (mFindDlg.IsWholeWord())
	{
		options |= QTextDocument::FindWholeWords;
	}
	mCurrentCursor = doc->find(mFindDlg.GetKeyword(), mCurrentCursor, options);
	if (!mCurrentCursor.isNull())
	{
		ui.plainTextEdit->setTextCursor(mCurrentCursor);
// 		QList<QTextEdit::ExtraSelection> extraSelections;//提供一种方式显示选择的文本
// 		//extraSelections = ui.plainTextEdit->extraSelections();//获取之前高亮的设置
// 		QTextEdit::ExtraSelection selection;
// 		//selection.format.setBackground(QColor(255,255,0));
// 		selection.format.setProperty(QTextFormat::FullWidthSelection, true);
// 		selection.cursor = mCurrentCursor;
// 		selection.cursor.movePosition(QTextCursor::Right,QTextCursor::MoveAnchor,mFindDlg.GetKeyword().size());//光标移动到某一行。此处移动到上一行，上一行将高亮。
// 		extraSelections.append(selection);
// 		ui.plainTextEdit->setExtraSelections(extraSelections);//设置高亮
	}
	
}

void QNotepad::InitShortcut()
{
	//文件
	ui.actionSave->setShortcut(tr("ctrl+S"));
	ui.actionNew->setShortcut(QKeySequence::New);
	ui.actionOpen->setShortcut(QKeySequence::Open);
	//编辑
	ui.actionUndo->setShortcut(QKeySequence::Undo);
	ui.actionCut->setShortcut(QKeySequence::Cut);
	ui.actionCopy->setShortcut(QKeySequence::Copy);
	ui.actionPaste->setShortcut(QKeySequence::Paste);
	ui.actionDelete->setShortcut(QKeySequence::Delete);
	ui.actionFind->setShortcut(QKeySequence::Find);
	ui.actionFindNext->setShortcut(QKeySequence::FindNext);
	ui.actionReplace->setShortcut(QKeySequence::Replace);
	ui.actionGo->setShortcut(tr("ctrl+G"));
	ui.actionAll->setShortcut(QKeySequence::SelectAll);
	ui.actionDate->setShortcut(tr("F5"));
}

void QNotepad::InitConnect()
{
	//文件
	connect(ui.plainTextEdit, SIGNAL(textChanged()), this, SLOT(UpdateFileStatus()));
	connect(ui.actionOpen, SIGNAL(triggered()), this, SLOT(OpenFile()));
	connect(ui.actionNew, SIGNAL(triggered()), this, SLOT(NewFile()));
	connect(ui.actionSave, SIGNAL(triggered()), this, SLOT(SaveFile()));
	connect(ui.actionSaveAs, SIGNAL(triggered()), this, SLOT(SaveAsFile()));
	connect(ui.actionExit, SIGNAL(triggered()), this, SLOT(close()));
	//编辑
	connect(ui.actionUndo, SIGNAL(triggered()), ui.plainTextEdit, SLOT(undo()));
	connect(ui.actionCut, SIGNAL(triggered()), ui.plainTextEdit, SLOT(cut()));
	connect(ui.actionCopy, SIGNAL(triggered()), ui.plainTextEdit, SLOT(copy()));
	connect(ui.actionPaste, SIGNAL(triggered()), ui.plainTextEdit, SLOT(paste()));
	//connect(ui.actionDelete, SIGNAL(triggered()), ui.plainTextEdit, SLOT(clear()));
	connect(ui.actionFind, SIGNAL(triggered()), this, SLOT(Find()));
	connect(ui.actionFindNext, SIGNAL(triggered()), this, SLOT(FindNext()));
	connect(ui.actionAll, SIGNAL(triggered()), ui.plainTextEdit, SLOT(selectAll()));
	connect(ui.actionDate, SIGNAL(triggered()), this, SLOT(InsertSysDataTime()));
}

bool QNotepad::PreCheck()
{
	QString promptInfo("\n" + mCurrentFilePath);
	if (mCurrentFilePath.isEmpty())
	{
		promptInfo = " " + defaultTitle;
	}
	QString text = QString(tr("是否将更改保存到%1?")).arg(promptInfo);
	int ret = QMessageBox::warning(this, tr("记事本"), text, tr("保存"), tr("不保存"), tr("取消"));
	if (ret == 0)
	{
		if (!SaveFile())
		{
			return false;
		}
	}
	else if (ret == 2)
	{
		return false;
	}
	return true;
}
