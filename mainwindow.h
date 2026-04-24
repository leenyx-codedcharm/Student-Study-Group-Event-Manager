#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QListWidget>
#include "studentmanager.h"
#include "groupmanager.h"
#include "eventmanager.h"
#include "taskmanager.h"
#include "filehandler.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    explicit MainWindow(QWidget* parent = nullptr);
    ~MainWindow();

private slots:
    // ── Landing page
    void on_btnIsStudent_clicked();
    void on_btnIsAdmin_clicked();
    void on_btnBackFromChoice_clicked();

    // ── Admin login
    void on_btnAdminLogin_clicked();
    void on_btnBackFromAdminLogin_clicked();

    // ── Student apply (self-register)
    void on_btnStudentApply_clicked();
    void on_btnBackFromApply_clicked();

    // ── Student login
    void on_btnStudentLogin_clicked();
    void on_btnBackFromStudentLogin_clicked();
    void on_btnGoToApply_clicked();
    void on_btnGoToStudentLogin_clicked();

    // ── Logout
    void on_btnAdminLogout_clicked();
    void on_btnStudentLogout_clicked();

    // ── Admin: students
    void on_btnAdminRegisterStudent_clicked();
    void on_btnAdminViewStudents_clicked();
    void on_btnAdminSearchStudent_clicked();
    void on_btnAdminDeleteStudent_clicked();
    void on_btnAdminBlockStudent_clicked();
    void on_btnAdminUnblockStudent_clicked();
    void on_btnAdminUpdateStudent_clicked();
    void on_btnAdminSortName_clicked();
    void on_btnAdminSortId_clicked();
    void on_btnAdminSortCGPA_clicked();
    void on_btnAdminReverseList_clicked();
    void on_btnAdminRemoveDuplicates_clicked();
    void on_btnAdminChangeCredentials_clicked();

    // ── Admin: groups
    void on_btnAdminCreateGroup_clicked();
    void on_btnAdminViewGroups_clicked();
    void on_btnAdminDeleteGroup_clicked();
    void on_btnAdminMergeGroups_clicked();

    // ── Admin: events
    void on_btnAdminViewAllEvents_clicked();
    void on_btnAdminViewPending_clicked();
    void on_btnAdminApproveEvent_clicked();
    void on_btnAdminDeleteEvent_clicked();

    // ── Admin: system
    void on_btnAdminStats_clicked();
    void on_btnAdminExportReport_clicked();
    void on_btnAdminBackup_clicked();
    void on_btnAdminRestore_clicked();

    // ── Student: profile
    void on_btnViewProfile_clicked();
    void on_btnStudentSaveProfile_clicked();
    void on_btnStudentChangePassword_clicked();

    // ── Student: groups
    void on_btnCreateGroup_clicked();
    void on_btnJoinGroup_clicked();
    void on_btnLeaveGroup_clicked();
    void on_btnViewMyGroups_clicked();
    void on_btnSearchGroups_clicked();
    void on_btnViewPublicGroups_clicked();
    void on_btnCloseGroup_clicked();
    void on_btnPromoteMember_clicked();
    void on_btnViewMembers_clicked();

    // ── Student: events
    void on_btnRequestEvent_clicked();
    void on_btnJoinEvent_clicked();
    void on_btnViewGroupEvents_clicked();
    void on_btnViewUpcomingEvents_clicked();
    void on_btnViewReminders_clicked();

    // ── Admin: tasks
    void on_btnAdminCreateTask_clicked();
    void on_btnAdminViewAllTasks_clicked();
    void on_btnAdminUpdateTaskProgress_clicked();
    void on_btnAdminMarkTaskDone_clicked();
    void on_btnAdminDeleteTask_clicked();
    void on_btnAdminViewGroupTasks_clicked();

    // ── Student: tasks
    void on_btnCreateTask_clicked();
    void on_btnUpdateProgress_clicked();
    void on_btnMarkTaskDone_clicked();
    void on_btnViewGroupTasks_clicked();
    void on_btnViewMyTasks_clicked();
    void on_btnViewPendingTasks_clicked();
    void on_btnSortTasksByDeadline_clicked();
    void on_listTasks_itemClicked(QListWidgetItem* item);
private:
    Ui::MainWindow* ui;
    StudentManager  students;
    GroupManager    groups;
    EventManager    events;
    TaskManager     tasks;
    FileHandler     fileHandler;
    StudentNode*    currentUser = nullptr;

    // CAPTCHA
    QString currentCaptcha;
    void    generateCaptcha(const QString& labelName, const QString& inputName);
    bool    verifyCaptcha(const QString& inputName);

    void autoSave();
    void applyStyles();

    // Page indices (matches stackedWidget order in .ui)
    enum Page {
        PAGE_LANDING      = 0,
        PAGE_ADMIN_LOGIN  = 1,
        PAGE_STUDENT_CHOICE = 2,
        PAGE_APPLY        = 3,
        PAGE_STUDENT_LOGIN = 4,
        PAGE_ADMIN_DASH   = 5,
        PAGE_STUDENT_DASH = 6
    };
    void goTo(int page);
};

#endif // MAINWINDOW_H
