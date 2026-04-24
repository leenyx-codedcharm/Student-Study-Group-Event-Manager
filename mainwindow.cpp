#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMessageBox>
#include <QDateTime>
#include <QFileDialog>
#include <QMap>
#include <QRandomGenerator>

// ── Constructor ───────────────────────────────────────────────────────────
MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent), ui(new Ui::MainWindow), fileHandler() {
    ui->setupUi(this);
    setWindowTitle("Let's Study Smart");
    resize(1200, 800);
    applyStyles();
    fileHandler.loadAll(students, groups, events, tasks);
    // Always ensure admin account exists (safe - skips if username taken)
    students.addAdmin("admin", "admin123", "System Administrator");
    // Also ensure the loaded admin has correct role (file might be stale)
    auto* adminNode = students.findByUsername("admin");
    if (adminNode) adminNode->role = "Admin";
    goTo(PAGE_LANDING);
}

MainWindow::~MainWindow() {
    fileHandler.saveAll(students, groups, events, tasks);
    delete ui;
}

void MainWindow::autoSave() { fileHandler.saveAll(students, groups, events, tasks); }
void MainWindow::goTo(int page) { ui->stackedWidget->setCurrentIndex(page); }

// ── CAPTCHA ───────────────────────────────────────────────────────────────
void MainWindow::generateCaptcha(const QString& labelName, const QString& inputName) {
    int n = QRandomGenerator::global()->bounded(1000, 9999);
    currentCaptcha = QString::number(n);
    // Direct ui references - more reliable than findChild across stacked pages
    if (labelName == "lblCaptchaAdmin")
        ui->lblCaptchaAdmin->setText("CAPTCHA: " + currentCaptcha);
    else if (labelName == "lblCaptchaStudent")
        ui->lblCaptchaStudent->setText("CAPTCHA: " + currentCaptcha);
    if (inputName == "txtAdminCaptcha")
        ui->txtAdminCaptcha->clear();
    else if (inputName == "txtStudentCaptcha")
        ui->txtStudentCaptcha->clear();
}

bool MainWindow::verifyCaptcha(const QString& inputName) {
    if (currentCaptcha.isEmpty()) return false;
    QString entered;
    if (inputName == "txtAdminCaptcha")
        entered = ui->txtAdminCaptcha->text().trimmed();
    else if (inputName == "txtStudentCaptcha")
        entered = ui->txtStudentCaptcha->text().trimmed();
    return entered == currentCaptcha;
}

// ── Styles ────────────────────────────────────────────────────────────────
void MainWindow::applyStyles() {
    setStyleSheet(R"(
        QMainWindow,QWidget{background:#0f172a;color:#f1f5f9;}
        QLabel{color:#f1f5f9;font-size:13px;}
        QLabel#lblAppTitle{font-size:28px;font-weight:bold;color:#60a5fa;}
        QLabel#lblAppSub{font-size:15px;color:#94a3b8;}
        QLabel#lblCaptchaAdmin,QLabel#lblCaptchaStudent{
            font-size:18px;font-weight:bold;
            color:#facc15;letter-spacing:4px;background:#0f2744;
            border:2px solid #facc15;border-radius:8px;
            padding:8px 14px;}
        QLineEdit,QComboBox,QSpinBox,QDoubleSpinBox{
            background:#0f1f35;color:#ffffff;border:2px solid #475569;
            border-radius:8px;padding:11px 18px;
            font-size:14px;font-weight:500;min-height:38px;}
        QLineEdit:focus{border:2px solid #60a5fa;background:#0a192f;color:#ffffff;}
        QLineEdit:hover{border:2px solid #64748b;}
        QComboBox::drop-down{border:none;}
        QSpinBox::up-button,QSpinBox::down-button{width:18px;}
        QListWidget{background:#0f172a;color:#e2e8f0;border:1px solid #334155;
            border-radius:8px;padding:6px;font-size:14px;}
        QListWidget::item{padding:7px 10px;border-radius:4px;border-bottom:1px solid #1e293b;}
        QListWidget::item:selected{background:#2563eb;color:#ffffff;}
        QListWidget::item:hover{background:#1e293b;}
        QPushButton{background:#2563eb;color:white;border:none;
            border-radius:8px;padding:8px 16px;font-size:13px;font-weight:bold;}
        QPushButton:hover{background:#1d4ed8;}
        QPushButton:pressed{background:#1e40af;}
        QPushButton#btnIsStudent,QPushButton#btnIsAdmin{
            font-size:16px;padding:16px 32px;border-radius:12px;min-width:160px;}
        QPushButton#btnIsAdmin{background:#7c3aed;}
        QPushButton#btnIsAdmin:hover{background:#6d28d9;}
        QPushButton#btnAdminLogin,QPushButton#btnStudentLogin{
            background:#16a34a;font-size:14px;padding:10px;}
        QPushButton#btnAdminLogin:hover,QPushButton#btnStudentLogin:hover{background:#15803d;}
        QPushButton#btnStudentApply{background:#0891b2;}
        QPushButton#btnAdminLogout,QPushButton#btnStudentLogout{background:#dc2626;}
        QTabWidget::pane{border:1px solid #334155;border-radius:8px;}
        QTabBar::tab{background:#1e293b;color:#94a3b8;padding:9px 18px;
            margin:2px;border-radius:6px;font-size:13px;}
        QTabBar::tab:selected{background:#2563eb;color:white;font-weight:bold;}
        QFrame#frameAdminLogin,QFrame#frameStudentLogin,
        QFrame#frameApply,QFrame#frameChoice{
            background:#1e293b;border:1px solid #334155;border-radius:16px;}
        QGroupBox{color:#94a3b8;border:1px solid #334155;border-radius:8px;
            margin-top:8px;padding:8px;}
        QGroupBox::title{subcontrol-origin:margin;left:12px;color:#60a5fa;}
    )");
}

// ════════════════════════════════════════════════════════════════════════
//  LANDING PAGE
// ════════════════════════════════════════════════════════════════════════
void MainWindow::on_btnIsStudent_clicked() { goTo(PAGE_STUDENT_CHOICE); }
void MainWindow::on_btnIsAdmin_clicked() {
    generateCaptcha("lblCaptchaAdmin", "txtAdminCaptcha");
    goTo(PAGE_ADMIN_LOGIN);
}
void MainWindow::on_btnBackFromChoice_clicked() { goTo(PAGE_LANDING); }

// ── Admin login ────────────────────────────────────────────────────────
void MainWindow::on_btnBackFromAdminLogin_clicked() { goTo(PAGE_LANDING); }

void MainWindow::on_btnAdminLogin_clicked() {
    if (!verifyCaptcha("txtAdminCaptcha")) {
        QMessageBox::warning(this,"CAPTCHA","Incorrect CAPTCHA. Please try again.");
        generateCaptcha("lblCaptchaAdmin","txtAdminCaptcha"); return;
    }
    QString un = ui->txtAdminUsername->text().trimmed();
    QString pw = ui->txtAdminPassword->text().trimmed();
    currentUser = students.login(un, pw);
    if (!currentUser || currentUser->role != "Admin") {
        currentUser = nullptr;
        QMessageBox::warning(this,"Login Failed","Invalid admin credentials.");
        generateCaptcha("lblCaptchaAdmin","txtAdminCaptcha"); return;
    }
    ui->txtAdminUsername->clear(); ui->txtAdminPassword->clear();
    ui->lblAdminWelcome->setText("👋 Welcome, " + currentUser->fullName);
    goTo(PAGE_ADMIN_DASH);
}

// ── Student choice page ───────────────────────────────────────────────
void MainWindow::on_btnGoToApply_clicked() { goTo(PAGE_APPLY); }
void MainWindow::on_btnGoToStudentLogin_clicked() {
    generateCaptcha("lblCaptchaStudent","txtStudentCaptcha");
    goTo(PAGE_STUDENT_LOGIN);
}

// ── Student Apply (self-register) ─────────────────────────────────────
void MainWindow::on_btnBackFromApply_clicked() { goTo(PAGE_STUDENT_CHOICE); }

void MainWindow::on_btnStudentApply_clicked() {
    QString sid   = ui->txtApplyId->text().trimmed();
    QString name  = ui->txtApplyName->text().trimmed();
    QString cgpaStr = ui->txtApplyCGPA->text().trimmed().replace(",", ".");
    double  cgpa  = cgpaStr.isEmpty() ? 0.0 : cgpaStr.toDouble();
    QString sem   = ui->txtApplySem->text().trimmed();
    QString dept  = ui->txtApplyDept->text().trimmed();
    QString email = ui->txtApplyEmail->text().trimmed();

    if (sid.isEmpty()||name.isEmpty()||email.isEmpty()||dept.isEmpty()||sem.isEmpty()) {
        QMessageBox::warning(this,"Error","All fields are required."); return;
    }
    if (cgpa < 0.0 || cgpa > 4.0) {
        QMessageBox::warning(this,"Error","CGPA must be between 0.0 and 4.0."); return;
    }
    if (sid.length() != 10 || !sid.toLongLong()) {
        QMessageBox::warning(this,"Error","Student ID must be exactly 10 digits."); return;
    }

    QString un = StudentManager::generateUsername(name, sid);
    QString pw = StudentManager::generatePassword(name);

    if (!students.registerStudent(sid, name, dept, sem, email, "", cgpa)) {
        QMessageBox::warning(this,"Error","Student ID already registered."); return;
    }
    autoSave();

    // Show credentials to new student
    QMessageBox::information(this, "✅ Registration Successful",
        QString("Welcome, %1!\n\n"
                "Your login credentials:\n"
                "  Username : %2\n"
                "  Password : %3\n\n"
                "Please remember these. You can change your password after login.")
        .arg(name).arg(un).arg(pw));

    ui->txtApplyId->clear(); ui->txtApplyName->clear();
    ui->txtApplyCGPA->clear(); ui->txtApplySem->clear();
    ui->txtApplyDept->clear(); ui->txtApplyEmail->clear();
    goTo(PAGE_STUDENT_CHOICE);
}

// ── Student Login ─────────────────────────────────────────────────────
void MainWindow::on_btnBackFromStudentLogin_clicked() { goTo(PAGE_STUDENT_CHOICE); }

void MainWindow::on_btnStudentLogin_clicked() {
    if (!verifyCaptcha("txtStudentCaptcha")) {
        QMessageBox::warning(this,"CAPTCHA","Incorrect CAPTCHA. Please try again.");
        generateCaptcha("lblCaptchaStudent","txtStudentCaptcha"); return;
    }
    QString un = ui->txtStudentUsername->text().trimmed();
    QString pw = ui->txtStudentPassword->text().trimmed();
    currentUser = students.login(un, pw);
    if (!currentUser || currentUser->role == "Admin") {
        currentUser = nullptr;
        QMessageBox::warning(this,"Login Failed","Invalid credentials or account blocked.");
        generateCaptcha("lblCaptchaStudent","txtStudentCaptcha"); return;
    }
    ui->txtStudentUsername->clear(); ui->txtStudentPassword->clear();
    ui->lblStudentWelcome->setText("👋 Welcome, " + currentUser->fullName
                                   + "  [" + currentUser->semester + " | "
                                   + currentUser->department + "]");
    goTo(PAGE_STUDENT_DASH);
}

// ── Logout ────────────────────────────────────────────────────────────
void MainWindow::on_btnAdminLogout_clicked()   { currentUser=nullptr; goTo(PAGE_LANDING); }
void MainWindow::on_btnStudentLogout_clicked() { currentUser=nullptr; goTo(PAGE_LANDING); }

// ════════════════════════════════════════════════════════════════════════
//  ADMIN – CREDENTIALS
// ════════════════════════════════════════════════════════════════════════
void MainWindow::on_btnAdminChangeCredentials_clicked() {
    if (!currentUser) return;
    QString curPw  = ui->txtAdminCurPw->text().trimmed();
    QString newUn  = ui->txtAdminNewUn->text().trimmed();
    QString newPw  = ui->txtAdminNewPw->text().trimmed();
    if (curPw.isEmpty()||newUn.isEmpty()||newPw.isEmpty()) {
        QMessageBox::warning(this,"Error","All fields required."); return;
    }
    if (students.adminChangeOwnCredentials(currentUser->studentId, newUn, newPw, curPw)) {
        currentUser = students.findByUsername(newUn);
        QMessageBox::information(this,"Updated","Admin credentials updated.");
        autoSave();
    } else QMessageBox::warning(this,"Error","Current password wrong or username taken.");
    ui->txtAdminCurPw->clear(); ui->txtAdminNewUn->clear(); ui->txtAdminNewPw->clear();
}

// ════════════════════════════════════════════════════════════════════════
//  ADMIN – REGISTER STUDENT
// ════════════════════════════════════════════════════════════════════════
void MainWindow::on_btnAdminRegisterStudent_clicked() {
    QString sid   = ui->txtRegSid->text().trimmed();
    QString name  = ui->txtRegName->text().trimmed();
    QString dept  = ui->txtRegDept->text().trimmed();
    QString sem   = ui->txtRegSem->text().trimmed();
    QString email = ui->txtRegEmail->text().trimmed();
    QString phone = ui->txtRegPhone->text().trimmed();
    double  cgpa  = ui->txtRegCGPA->text().trimmed().replace(",",".").toDouble();

    if (sid.isEmpty()||name.isEmpty()||dept.isEmpty()||sem.isEmpty()||email.isEmpty()) {
        QMessageBox::warning(this,"Error","All required fields must be filled."); return;
    }
    if (!students.registerStudent(sid, name, dept, sem, email, phone, cgpa)) {
        QMessageBox::warning(this,"Error","Student ID already exists."); return;
    }
    QString un = StudentManager::generateUsername(name, sid);
    QString pw = StudentManager::generatePassword(name);
    autoSave();
    QMessageBox::information(this,"Registered!",
        QString("Student registered!\n\nUsername : %1\nPassword : %2").arg(un).arg(pw));
    ui->txtRegSid->clear(); ui->txtRegName->clear(); ui->txtRegDept->clear();
    ui->txtRegSem->clear(); ui->txtRegEmail->clear(); ui->txtRegPhone->clear(); ui->txtRegCGPA->clear();
}

// ════════════════════════════════════════════════════════════════════════
//  ADMIN – MANAGE STUDENTS
// ════════════════════════════════════════════════════════════════════════
void MainWindow::on_btnAdminViewStudents_clicked() {
    ui->listAdminStudents->clear();
    auto list = students.getAllFormatted();
    ui->listAdminStudents->addItems(list.isEmpty() ? QStringList{"No students."} : list);
}
void MainWindow::on_btnAdminSearchStudent_clicked() {
    QString kw = ui->txtAdminSearch->text().trimmed(); if(kw.isEmpty()) return;
    ui->listAdminStudents->clear();
    auto list = students.searchByName(kw);
    ui->listAdminStudents->addItems(list.isEmpty() ? QStringList{"Not found."} : list);
}
void MainWindow::on_btnAdminDeleteStudent_clicked() {
    QString id = ui->txtAdminStudentId->text().trimmed();
    if (students.deleteStudent(id)) { QMessageBox::information(this,"Deleted","Student deleted."); autoSave(); }
    else QMessageBox::warning(this,"Error","Not found or cannot delete admin.");
}
void MainWindow::on_btnAdminBlockStudent_clicked() {
    QString id = ui->txtAdminStudentId->text().trimmed();
    if (students.blockStudent(id)) { QMessageBox::information(this,"Blocked","Student blocked."); autoSave(); }
    else QMessageBox::warning(this,"Error","Not found or cannot block admin.");
}
void MainWindow::on_btnAdminUnblockStudent_clicked() {
    QString id = ui->txtAdminStudentId->text().trimmed();
    if (students.unblockStudent(id)) { QMessageBox::information(this,"Unblocked","Student unblocked."); autoSave(); }
    else QMessageBox::warning(this,"Error","Student not found.");
}
void MainWindow::on_btnAdminUpdateStudent_clicked() {
    QString sid    = ui->txtAdminUpdateSid->text().trimmed();  // from Update tab
    QString name   = ui->txtAdminUpdateName->text().trimmed();
    QString newSid = ui->txtAdminUpdateNewSid->text().trimmed();
    QString un     = ui->txtAdminUpdateUn->text().trimmed();
    QString email  = ui->txtAdminUpdateEmail->text().trimmed();
    QString dept   = ui->txtAdminUpdateDept->text().trimmed();
    QString sem    = ui->txtAdminUpdateSem->text().trimmed();
    QString phone  = ui->txtAdminUpdatePhone->text().trimmed();
    double  cgpa   = ui->txtAdminUpdateCGPA->text().trimmed().replace(",",".").toDouble();
    if (sid.isEmpty()||name.isEmpty()||newSid.isEmpty()||un.isEmpty()) {
        QMessageBox::warning(this,"Error","Student ID, Name, New ID and Username are required."); return;
    }
    if (students.adminUpdateStudent(sid,name,newSid,un,email,dept,sem,phone,cgpa)) {
        QMessageBox::information(this,"Updated","Student updated."); autoSave();
    } else QMessageBox::warning(this,"Error","ID or username conflict.");
}
void MainWindow::on_btnAdminSortName_clicked()       { students.sortByName();  on_btnAdminViewStudents_clicked(); }
void MainWindow::on_btnAdminSortId_clicked()         { students.sortById();    on_btnAdminViewStudents_clicked(); }
void MainWindow::on_btnAdminSortCGPA_clicked()       { students.sortByCGPA();  on_btnAdminViewStudents_clicked(); }
void MainWindow::on_btnAdminReverseList_clicked()    { students.reverseList(); on_btnAdminViewStudents_clicked(); }
void MainWindow::on_btnAdminRemoveDuplicates_clicked(){ students.removeDuplicates(); on_btnAdminViewStudents_clicked(); autoSave(); }

// ════════════════════════════════════════════════════════════════════════
//  ADMIN – GROUPS
// ════════════════════════════════════════════════════════════════════════
void MainWindow::on_btnAdminViewGroups_clicked() {
    ui->listAdminGroups->clear();
    // Build name map: studentId -> fullName
    QMap<QString,QString> nameMap;
    for (auto* s = students.getHead(); s; s = s->next)
        nameMap[s->studentId] = s->fullName;
    auto list = groups.getAllDetailed(nameMap);
    ui->listAdminGroups->addItems(list.isEmpty() ? QStringList{"No groups."} : list);
}
void MainWindow::on_btnAdminDeleteGroup_clicked() {
    QString id = ui->txtAdminGroupId->text().trimmed().toUpper();
    if (!id.startsWith("SG")) id = "SG" + id;
    if (groups.deleteGroup(id)) { QMessageBox::information(this,"Deleted","Group deleted."); autoSave(); }
    else QMessageBox::warning(this,"Error","Group not found.");
}
void MainWindow::on_btnAdminMergeGroups_clicked() {
    QString id1=ui->txtMergeGroup1->text().trimmed().toUpper();
    QString id2=ui->txtMergeGroup2->text().trimmed().toUpper();
    if (!id1.startsWith("SG")) id1="SG"+id1;
    if (!id2.startsWith("SG")) id2="SG"+id2;
    if (!groups.findById(id1)||!groups.findById(id2)) {
        QMessageBox::warning(this,"Error","One or both groups not found."); return;
    }
    groups.mergeGroups(id1,id2);
    QMessageBox::information(this,"Merged",QString("Group %1 merged into %2. Group %1 deleted.").arg(id2).arg(id1));
    autoSave(); on_btnAdminViewGroups_clicked();
}

// ════════════════════════════════════════════════════════════════════════
//  ADMIN – EVENTS (Approve / Delete)
// ════════════════════════════════════════════════════════════════════════
void MainWindow::on_btnAdminViewAllEvents_clicked() {
    ui->listAdminEvents->clear();
    auto list = events.getAll();
    ui->listAdminEvents->addItems(list.isEmpty() ? QStringList{"No events."} : list);
}
void MainWindow::on_btnAdminViewPending_clicked() {
    ui->listAdminEvents->clear();
    auto list = events.getPendingEvents();
    ui->listAdminEvents->addItems(list.isEmpty() ? QStringList{"No pending event requests."} : list);
}
void MainWindow::on_btnAdminApproveEvent_clicked() {
    int id = ui->txtAdminEventId->text().toInt();
    auto* e = events.findById(id);
    if (!e) { QMessageBox::warning(this,"Error","Event not found."); return; }
    if (e->status != "Pending") { QMessageBox::warning(this,"Error","Event is not pending."); return; }
    events.approveEvent(id);
    groups.incrementApprovedEvents(e->groupId);
    QMessageBox::information(this,"Approved","Event approved! Students can now join it.");
    autoSave(); on_btnAdminViewPending_clicked();
}
void MainWindow::on_btnAdminDeleteEvent_clicked() {
    int id = ui->txtAdminEventId->text().toInt();
    if (events.findById(id)) {
        events.deleteEvent(id);
        QMessageBox::information(this,"Deleted","Event deleted."); autoSave();
        on_btnAdminViewAllEvents_clicked();
    } else QMessageBox::warning(this,"Error","Event not found.");
}

// ════════════════════════════════════════════════════════════════════════
//  ADMIN – STATS
// ════════════════════════════════════════════════════════════════════════
void MainWindow::on_btnAdminStats_clicked() {
    auto* ts = students.getMostActive();
    auto* tg = groups.getMostActive();
    QMessageBox::information(this,"Statistics",
        QString("Total Students    : %1\nTotal Groups      : %2\n"
                "Total Events      : %3\nUpcoming Events   : %4\n"
                "Total Tasks       : %5\nTasks Completed   : %6%\n\n"
                "Most Active Student: %7\nMost Active Group  : %8")
        .arg(students.totalStudents()).arg(groups.totalGroups())
        .arg(events.totalEvents()).arg(events.upcomingCount())
        .arg(tasks.totalTasks()).arg(tasks.completedPercent(),0,'f',1)
        .arg(ts ? ts->fullName+" ("+QString::number(ts->joinedGroups)+" groups)" : "N/A")
        .arg(tg ? tg->name+" ("+QString::number(tg->currentMembers)+" members)" : "N/A"));
}
void MainWindow::on_btnAdminExportReport_clicked() {
    QString path = QFileDialog::getSaveFileName(this,"Export","","Text Files (*.txt)");
    if (path.isEmpty()) return;
    if (fileHandler.exportReportTxt(students,groups,events,tasks,path))
        QMessageBox::information(this,"Exported","Report saved:\n"+path);
    else QMessageBox::warning(this,"Error","Could not save.");
}
void MainWindow::on_btnAdminBackup_clicked() {
    if (fileHandler.backupSystem()) QMessageBox::information(this,"Backup","Backup created.");
    else QMessageBox::warning(this,"Error","Backup failed.");
}
void MainWindow::on_btnAdminRestore_clicked() {
    if (QMessageBox::question(this,"Restore","Restore from latest backup?")==QMessageBox::Yes)
        fileHandler.restoreSystem(students,groups,events,tasks)
            ? QMessageBox::information(this,"Restored","Data restored.")
            : QMessageBox::warning(this,"Error","No backup found.");
}

// ════════════════════════════════════════════════════════════════════════
//  ADMIN – CREATE GROUP
// ════════════════════════════════════════════════════════════════════════
void MainWindow::on_btnAdminCreateGroup_clicked() {
    if (!currentUser) return;
    QString name   = ui->txtAdminGroupName->text().trimmed();
    QString course = ui->txtAdminCourseName->text().trimmed();
    QString code   = ui->txtAdminCourseCode->text().trimmed();
    QString desc   = ui->txtAdminGroupDesc->text().trimmed();
    int     maxM   = ui->spnAdminMaxMembers->value();
    QString priv   = ui->cmbAdminPrivacy->currentText();
    QString date   = QDateTime::currentDateTime().toString("yyyy-MM-dd");
    if (name.isEmpty()||course.isEmpty()) {
        QMessageBox::warning(this,"Error","Group name and course name are required."); return;
    }
    QString gid = groups.createGroup(name,course,code,desc,"ADMIN",maxM,priv,date);
    autoSave();
    QMessageBox::information(this,"Group Created!","Admin group created! Group ID: " + gid + " | Share this ID with students.");
    ui->txtAdminGroupName->clear(); ui->txtAdminCourseName->clear();
    ui->txtAdminCourseCode->clear(); ui->txtAdminGroupDesc->clear();
}

// ════════════════════════════════════════════════════════════════════════
//  ADMIN – TASK MANAGEMENT
// ════════════════════════════════════════════════════════════════════════
void MainWindow::on_btnAdminCreateTask_clicked() {
    QString gid      = ui->txtAdminTaskGroupId->text().trimmed().toUpper();
    if (!gid.startsWith("SG")) gid = "SG" + gid;
    QString title    = ui->txtAdminTaskTitle->text().trimmed();
    QString deadline = ui->txtAdminTaskDeadline->text().trimmed();
    QString memberId = ui->txtAdminTaskMemberId->text().trimmed();
    QString notes    = ui->txtAdminTaskNotes->text().trimmed();
    if (title.isEmpty()||deadline.isEmpty()||gid.isEmpty()) {
        QMessageBox::warning(this,"Error","Group ID, title and deadline are required."); return;
    }
    if (!groups.findById(gid)) {
        QMessageBox::warning(this,"Error","Group not found."); return;
    }
    tasks.createTask(gid,title,deadline,memberId,"ADMIN",notes);
    QMessageBox::information(this,"Created","Task assigned by Admin."); autoSave();
    ui->txtAdminTaskTitle->clear(); ui->txtAdminTaskDeadline->clear();
    ui->txtAdminTaskMemberId->clear(); ui->txtAdminTaskNotes->clear();
}

void MainWindow::on_btnAdminViewAllTasks_clicked() {
    ui->listAdminTasks->clear();
    auto list = tasks.getAllDetailed();
    ui->listAdminTasks->addItems(list.isEmpty() ? QStringList{"No tasks."} : list);
}

void MainWindow::on_btnAdminViewGroupTasks_clicked() {
    QString gid = ui->txtAdminTaskViewGroupId->text().trimmed().toUpper();
    if (!gid.startsWith("SG")) gid = "SG" + gid;
    ui->listAdminTasks->clear();
    auto list = tasks.getByGroupDetailed(gid);
    ui->listAdminTasks->addItems(list.isEmpty() ? QStringList{"No tasks for this group."} : list);
}

void MainWindow::on_btnAdminUpdateTaskProgress_clicked() {
    int id  = ui->txtAdminTaskId->text().toInt();
    int pct = ui->spnAdminProgress->value();
    if (tasks.updateProgress(id,pct)) {
        QMessageBox::information(this,"Updated","Task progress updated."); autoSave();
        on_btnAdminViewAllTasks_clicked();
    } else QMessageBox::warning(this,"Error","Task not found.");
}

void MainWindow::on_btnAdminMarkTaskDone_clicked() {
    int id = ui->txtAdminTaskId->text().toInt();
    if (tasks.markCompleted(id)) {
        QMessageBox::information(this,"Done","Task marked completed."); autoSave();
        on_btnAdminViewAllTasks_clicked();
    } else QMessageBox::warning(this,"Error","Task not found.");
}

void MainWindow::on_btnAdminDeleteTask_clicked() {
    int id = ui->txtAdminTaskId->text().toInt();
    if (tasks.deleteTask(id)) {
        QMessageBox::information(this,"Deleted","Task deleted."); autoSave();
        on_btnAdminViewAllTasks_clicked();
    } else QMessageBox::warning(this,"Error","Task not found.");
}

// ════════════════════════════════════════════════════════════════════════
//  STUDENT – PROFILE
// ════════════════════════════════════════════════════════════════════════
void MainWindow::on_btnViewProfile_clicked() {
    if (!currentUser) return;
    QMessageBox::information(this,"My Profile",
        QString("ID         : %1\nName       : %2\nUsername   : %3\n"
                "Department : %4\nSemester   : %5\nEmail      : %6\n"
                "Phone      : %7\nCGPA       : %8\nGroups     : %9\n"
                "Last Login : %10\nStatus     : %11")
        .arg(currentUser->studentId).arg(currentUser->fullName).arg(currentUser->username)
        .arg(currentUser->department).arg(currentUser->semester).arg(currentUser->email)
        .arg(currentUser->phone).arg(currentUser->cgpa,0,'f',2)
        .arg(currentUser->joinedGroups).arg(currentUser->lastLogin).arg(currentUser->status));
}
void MainWindow::on_btnStudentSaveProfile_clicked() {
    if (!currentUser) return;
    QString dept  = ui->txtStudEditDept->text().trimmed();
    QString sem   = ui->txtStudEditSem->text().trimmed();
    QString phone = ui->txtStudEditPhone->text().trimmed();
    double  cgpa  = ui->txtStudEditCGPA->text().trimmed().replace(",",".").toDouble();
    students.studentUpdateProfile(currentUser->studentId, dept, sem, phone, cgpa);
    currentUser = students.findById(currentUser->studentId);
    QMessageBox::information(this,"Saved","Profile updated. Note: Name, ID, Email and Username can only be changed by Admin.");
    autoSave();
}
void MainWindow::on_btnStudentChangePassword_clicked() {
    if (!currentUser) return;
    QString oldPw = ui->txtStudOldPw->text().trimmed();
    QString newPw = ui->txtStudNewPw->text().trimmed();
    if (oldPw.isEmpty()||newPw.isEmpty()) { QMessageBox::warning(this,"Error","Both fields required."); return; }
    if (students.changePassword(currentUser->studentId, oldPw, newPw))
        { QMessageBox::information(this,"Done","Password changed."); autoSave(); }
    else QMessageBox::warning(this,"Error","Current password is incorrect.");
    ui->txtStudOldPw->clear(); ui->txtStudNewPw->clear();
}

// ════════════════════════════════════════════════════════════════════════
//  STUDENT – GROUPS
// ════════════════════════════════════════════════════════════════════════
void MainWindow::on_btnCreateGroup_clicked() {
    if (!currentUser) return;
    // CGPA requirement: minimum 3.00 to create a group
    if (currentUser->cgpa < 3.00) {
        QMessageBox::warning(this,"Not Eligible","Minimum CGPA of 3.00 is required to create a group.\nYour CGPA: " + QString::number(currentUser->cgpa,'f',2));
        return;
    }
    QString name   = ui->txtGroupName->text().trimmed();
    QString course = ui->txtCourseName->text().trimmed();
    QString code   = ui->txtCourseCode->text().trimmed();
    QString desc   = ui->txtGroupDesc->text().trimmed();
    int     maxM   = ui->spnMaxMembers->value();
    QString priv   = ui->cmbPrivacy->currentText();
    QString date   = QDateTime::currentDateTime().toString("yyyy-MM-dd");
    if (name.isEmpty()||course.isEmpty()) {
        QMessageBox::warning(this,"Error","Group name and course name are required."); return;
    }
    QString gid = groups.createGroup(name,course,code,desc,currentUser->studentId,maxM,priv,date);
    students.incrementGroupCount(currentUser->studentId);
    autoSave();
    QMessageBox::information(this,"Group Created!",
        QString("Study group created!\n\nGroup ID: %1\nGroup Name: %2\nCourse: %3\n\nShare Group ID '%1' with others.")
        .arg(gid).arg(name).arg(course));
    ui->txtGroupName->clear(); ui->txtCourseName->clear();
    ui->txtCourseCode->clear(); ui->txtGroupDesc->clear();
}
void MainWindow::on_btnJoinGroup_clicked() {
    if (!currentUser) return;
    QString id = ui->txtJoinGroupId->text().trimmed().toUpper();
    if (!id.startsWith("SG")) id = "SG" + id;
    if (!groups.findById(id)) { QMessageBox::warning(this,"Error","Group not found. Use format SG001."); return; }
    if (groups.isMember(id, currentUser->studentId)) {
        QMessageBox::warning(this,"Error","You are already a member."); return;
    }
    if (groups.addMember(id, currentUser->studentId)) {
        students.incrementGroupCount(currentUser->studentId);
        QMessageBox::information(this,"Joined","You joined the group!"); autoSave();
    } else QMessageBox::warning(this,"Error","Group is full or closed.");
}
void MainWindow::on_btnLeaveGroup_clicked() {
    if (!currentUser) return;
    QString id = ui->txtLeaveGroupId->text().trimmed().toUpper();
    if (!id.startsWith("SG")) id = "SG" + id;
    if (groups.removeMember(id, currentUser->studentId)) {
        students.decrementGroupCount(currentUser->studentId);
        QMessageBox::information(this,"Left","You left the group."); autoSave();
    } else QMessageBox::warning(this,"Error","You are not a member.");
}
void MainWindow::on_btnViewMyGroups_clicked() {
    if (!currentUser) return;
    ui->listMyGroups->clear();
    QStringList found;
    for (auto* g=groups.getHead(); g; g=g->next)
        if (groups.isMember(g->groupId, currentUser->studentId))
            found << QString("[%1] %2 | %3 | %4").arg(g->groupId).arg(g->name).arg(g->courseName).arg(g->status);
    ui->listMyGroups->addItems(found.isEmpty() ? QStringList{"No groups joined."} : found);
}
void MainWindow::on_btnSearchGroups_clicked() {
    QString kw = ui->txtSearchGroup->text().trimmed();
    ui->listGroupSearch->clear();
    QStringList combined = groups.searchByName(kw) + groups.searchByCourse(kw);
    combined.removeDuplicates();
    ui->listGroupSearch->addItems(combined.isEmpty() ? QStringList{"No results."} : combined);
}
void MainWindow::on_btnViewPublicGroups_clicked() {
    ui->listGroupSearch->clear();
    auto list = groups.getPublicGroups();
    ui->listGroupSearch->addItems(list.isEmpty() ? QStringList{"No public groups."} : list);
}
void MainWindow::on_btnCloseGroup_clicked() {
    if (!currentUser) return;
    QString id = ui->txtManageGroupId->text().trimmed().toUpper();
    if (!id.startsWith("SG")) id = "SG" + id;
    auto* g = groups.findById(id);
    if (!g || g->leaderStudentId != currentUser->studentId) {
        QMessageBox::warning(this,"Error","Not found or you are not the leader."); return;
    }
    groups.closeGroup(id); QMessageBox::information(this,"Closed","Group closed."); autoSave();
}
void MainWindow::on_btnPromoteMember_clicked() {
    if (!currentUser) return;
    QString gid = ui->txtManageGroupId->text().trimmed().toUpper();
    if (!gid.startsWith("SG")) gid = "SG" + gid;
    QString sid = ui->txtPromoteMemberId->text().trimmed();
    auto* g = groups.findById(gid);
    if (!g || g->leaderStudentId != currentUser->studentId) {
        QMessageBox::warning(this,"Error","Not found or you are not the leader."); return;
    }
    groups.promoteToLeader(gid, sid);
    QMessageBox::information(this,"Promoted","Member promoted to leader."); autoSave();
}
void MainWindow::on_btnViewMembers_clicked() {
    QString id = ui->txtManageGroupId->text().trimmed().toUpper();
    if (!id.startsWith("SG")) id = "SG" + id;
    ui->listMembers->clear();
    QMap<QString,QString> nameMap;
    for (auto* s = students.getHead(); s; s = s->next)
        nameMap[s->studentId] = s->fullName;
    auto list = groups.getMembersFormatted(id, nameMap);
    ui->listMembers->addItems(list.isEmpty() ? QStringList{"Group not found."} : list);
}

// ════════════════════════════════════════════════════════════════════════
//  STUDENT – EVENTS
// ════════════════════════════════════════════════════════════════════════
void MainWindow::on_btnRequestEvent_clicked() {
    if (!currentUser) return;
    // CGPA check
    if (currentUser->cgpa < 3.5) {
        QMessageBox::warning(this,"Not Eligible",
            "Only students with CGPA above 3.5 can request events."); return;
    }
    QString gid   = ui->txtEventGroupId->text().trimmed().toUpper();
    if (!gid.startsWith("SG")) gid = "SG" + gid;
    QString title = ui->txtEventTitle->text().trimmed();
    QString date  = ui->txtEventDate->text().trimmed();
    QString time  = ui->txtEventTime->text().trimmed();
    QString venue = ui->txtEventVenue->text().trimmed();
    QString desc  = ui->txtEventDesc->text().trimmed();
    QString prio  = ui->cmbEventPriority->currentText();

    if (!groups.isMember(gid, currentUser->studentId)) {
        QMessageBox::warning(this,"Error","You must be a member of this group."); return;
    }
    // Max 3 approved events per group
    if (!groups.canRequestMoreEvents(gid)) {
        QMessageBox::warning(this,"Limit Reached","This group has reached the maximum of 3 approved events."); return;
    }
    if (title.isEmpty()||date.isEmpty()) {
        QMessageBox::warning(this,"Error","Title and date are required."); return;
    }
    events.requestEvent(gid, title, date, time, venue, desc, prio, currentUser->studentId);
    QMessageBox::information(this,"Requested",
        "Event request submitted!\nAdmin will review and approve it.");
    autoSave();
    ui->txtEventTitle->clear(); ui->txtEventDate->clear();
    ui->txtEventTime->clear(); ui->txtEventVenue->clear(); ui->txtEventDesc->clear();
}

void MainWindow::on_btnJoinEvent_clicked() {
    if (!currentUser) return;
    int id = ui->txtJoinEventId->text().toInt();
    auto* e = events.findById(id);
    if (!e) { QMessageBox::warning(this,"Error","Event not found."); return; }
    if (e->status != "Approved") {
        QMessageBox::warning(this,"Error","You can only join approved events."); return;
    }
    if (events.hasJoined(id, currentUser->studentId)) {
        QMessageBox::warning(this,"Error","You already joined this event."); return;
    }
    events.joinEvent(id, currentUser->studentId);
    QMessageBox::information(this,"Joined","You joined the event!"); autoSave();
}

void MainWindow::on_btnViewGroupEvents_clicked() {
    QString gid = ui->txtViewEventGroupId->text().trimmed().toUpper();
    if (!gid.startsWith("SG")) gid = "SG" + gid;
    ui->listEvents->clear();
    auto list = events.getApprovedByGroup(gid);
    ui->listEvents->addItems(list.isEmpty() ? QStringList{"No approved events for this group."} : list);
}
void MainWindow::on_btnViewUpcomingEvents_clicked() {
    ui->listEvents->clear();
    auto list = events.getUpcoming();
    ui->listEvents->addItems(list.isEmpty() ? QStringList{"No upcoming events."} : list);
}
void MainWindow::on_btnViewReminders_clicked() {
    events.buildReminderList();
    ui->listEvents->clear();
    auto list = events.getReminderList();
    ui->listEvents->addItems(list.isEmpty() ? QStringList{"No reminders."} : list);
}

// ════════════════════════════════════════════════════════════════════════
//  STUDENT – TASKS
// ════════════════════════════════════════════════════════════════════════
void MainWindow::on_btnCreateTask_clicked() {
    if (!currentUser) return;
    QString gid   = ui->txtTaskGroupId->text().trimmed().toUpper();
    if (!gid.startsWith("SG")) gid = "SG" + gid;
    QString title    = ui->txtTaskTitle->text().trimmed();
    QString deadline = ui->txtTaskDeadline->text().trimmed();
    QString memberId = ui->txtTaskMemberId->text().trimmed();
    QString notes    = ui->txtTaskNotes->text().trimmed();
    if (title.isEmpty()||deadline.isEmpty()) {
        QMessageBox::warning(this,"Error","Title and deadline required."); return;
    }
    auto* g = groups.findById(gid);
    if (!g) { QMessageBox::warning(this,"Error","Group not found."); return; }
    // Only group leader can assign tasks
    if (g->leaderStudentId != currentUser->studentId) {
        QMessageBox::warning(this,"Not Allowed",
            "Only the group leader can assign tasks."); return;
    }
    if (!groups.isMember(gid, memberId)) {
        QMessageBox::warning(this,"Error","Assigned student is not a member of this group."); return;
    }
    tasks.createTask(gid,title,deadline,memberId,currentUser->studentId,notes);
    QMessageBox::information(this,"Created","Task assigned successfully."); autoSave();
    ui->txtTaskTitle->clear(); ui->txtTaskDeadline->clear();
    ui->txtTaskMemberId->clear(); ui->txtTaskNotes->clear();
}
void MainWindow::on_btnUpdateProgress_clicked() {
    QString raw = ui->txtTaskId->text().trimmed();
    if (raw.startsWith("T", Qt::CaseInsensitive)) raw = raw.mid(1); // strip "T"
    int id = raw.toInt();
    int pct = ui->spnProgress->value();
    if (tasks.updateProgress(id, pct)) {
        QMessageBox::information(this, "Updated", "Progress updated.");
        autoSave();
    } else QMessageBox::warning(this, "Error", "Task not found.");
}

void MainWindow::on_btnMarkTaskDone_clicked() {
    QString raw = ui->txtTaskId->text().trimmed();
    if (raw.startsWith("T", Qt::CaseInsensitive)) raw = raw.mid(1);
    int id = raw.toInt();
    if (tasks.markCompleted(id)) {
        QMessageBox::information(this, "Done", "Task completed.");
        autoSave();
    } else QMessageBox::warning(this, "Error", "Task not found.");
}
void MainWindow::on_btnViewGroupTasks_clicked() {
    QString gid=ui->txtViewTaskGroupId->text().trimmed().toUpper();
    if (!gid.startsWith("SG")) gid = "SG" + gid;
    // Student can only see tasks if member of the group
    if (currentUser && currentUser->role != "Admin" && !groups.isMember(gid, currentUser->studentId)) {
        QMessageBox::warning(this,"Access Denied","You must be a member of this group to view tasks."); return;
    }
    ui->listTasks->clear();
    auto list=tasks.getByGroupDetailed(gid);
    ui->listTasks->addItems(list.isEmpty() ? QStringList{"No tasks."} : list);
}
void MainWindow::on_btnViewMyTasks_clicked() {
    if (!currentUser) return; ui->listTasks->clear();
    auto list=tasks.getByMember(currentUser->studentId);
    ui->listTasks->addItems(list.isEmpty() ? QStringList{"No tasks assigned to you."} : list);
}
void MainWindow::on_btnViewPendingTasks_clicked() {
    ui->listTasks->clear();
    auto list=tasks.getPending();
    ui->listTasks->addItems(list.isEmpty() ? QStringList{"No pending tasks."} : list);
}
void MainWindow::on_btnSortTasksByDeadline_clicked() {
    tasks.sortByDeadline(); on_btnViewPendingTasks_clicked();
}
void MainWindow::on_listTasks_itemClicked(QListWidgetItem* item) {
    QString text = item->text();  // e.g. "[T1] Note all linked list..."
    if (text.startsWith("[T")) {
        int end = text.indexOf("]");
        QString idStr = text.mid(2, end - 2);  // extracts "1" from "[T1]"
        ui->txtTaskId->setText(idStr);          // sets field to "1"
    }
}
