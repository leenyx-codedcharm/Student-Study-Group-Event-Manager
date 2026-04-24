#include "filehandler.h"
#include <QFile>
#include <QTextStream>
#include <QDir>
#include <QDateTime>

FileHandler::FileHandler(const QString& dir) {
    dataDir = dir.isEmpty() ? QDir::homePath() + "/StudyGroupData" : dir;
}
void FileHandler::ensureDataDir() { QDir().mkpath(dataDir); QDir().mkpath(backupDir()); }
QString FileHandler::studentsFile() const { return dataDir+"/students.txt"; }
QString FileHandler::groupsFile()   const { return dataDir+"/groups.txt"; }
QString FileHandler::eventsFile()   const { return dataDir+"/events.txt"; }
QString FileHandler::tasksFile()    const { return dataDir+"/tasks.txt"; }
QString FileHandler::backupDir()    const { return dataDir+"/backup"; }

bool FileHandler::saveStudents(StudentManager& sm) {
    QFile f(studentsFile()); if(!f.open(QIODevice::WriteOnly|QIODevice::Text)) return false;
    QTextStream out(&f);
    for (auto* s=sm.getHead(); s; s=s->next)
        out<<s->studentId<<"|"<<s->fullName<<"|"<<s->username<<"|"<<s->password<<"|"
           <<s->department<<"|"<<s->semester<<"|"<<s->email<<"|"<<s->phone<<"|"
           <<s->cgpa<<"|"<<s->role<<"|"<<s->joinedGroups<<"|"<<s->lastLogin<<"|"<<s->status<<"\n";
    return true;
}

bool FileHandler::saveGroups(GroupManager& gm) {
    QFile f(groupsFile()); if(!f.open(QIODevice::WriteOnly|QIODevice::Text)) return false;
    QTextStream out(&f);
    out<<"NEXTSEQ|"<<gm.getNextSeq()<<"\n";
    for (auto* g=gm.getHead(); g; g=g->next) {
        out<<"GROUP|"<<g->groupId<<"|"<<g->name<<"|"<<g->courseName<<"|"<<g->courseCode<<"|"
           <<g->description<<"|"<<g->leaderStudentId<<"|"<<g->maxMembers<<"|"
           <<g->currentMembers<<"|"<<g->privacy<<"|"<<g->creationDate<<"|"
           <<g->status<<"|"<<g->approvedEventCount<<"\n";
        for (auto* m=g->members; m; m=m->next)
            out<<"MEMBER|"<<g->groupId<<"|"<<m->studentId<<"|"<<m->role<<"\n";
    }
    return true;
}

bool FileHandler::saveEvents(EventManager& em) {
    QFile f(eventsFile()); if(!f.open(QIODevice::WriteOnly|QIODevice::Text)) return false;
    QTextStream out(&f);
    out<<"NEXTID|"<<em.getNextId()<<"\n";
    for (auto* e=em.getHead(); e; e=e->next) {
        out<<"EVENT|"<<e->id<<"|"<<e->groupId<<"|"<<e->title<<"|"<<e->date<<"|"
           <<e->time<<"|"<<e->venue<<"|"<<e->description<<"|"<<e->priority<<"|"
           <<e->status<<"|"<<(e->reminderEnabled?1:0)<<"|"<<e->requestedBy<<"\n";
        for (const QString& sid : e->joinedStudents)
            out<<"JOINEVENT|"<<e->id<<"|"<<sid<<"\n";
    }
    return true;
}

bool FileHandler::saveTasks(TaskManager& tm) {
    QFile f(tasksFile()); if(!f.open(QIODevice::WriteOnly|QIODevice::Text)) return false;
    QTextStream out(&f);
    out<<"NEXTID|"<<tm.getNextId()<<"\n";
    for (auto* t=tm.getHead(); t; t=t->next)
        out<<t->id<<"|"<<t->groupId<<"|"<<t->title<<"|"<<t->deadline<<"|"
           <<t->assignedMemberId<<"|"<<t->assignedBy<<"|"<<t->progressPercent<<"|"
           <<t->notes<<"|"<<t->status<<"\n";
    return true;
}

void FileHandler::saveAll(StudentManager& sm, GroupManager& gm, EventManager& em, TaskManager& tm) {
    ensureDataDir(); saveStudents(sm); saveGroups(gm); saveEvents(em); saveTasks(tm);
}

bool FileHandler::loadStudents(StudentManager& sm) {
    QFile f(studentsFile()); if(!f.open(QIODevice::ReadOnly|QIODevice::Text)) return false;
    QTextStream in(&f);
    while (!in.atEnd()) {
        QString line=in.readLine().trimmed(); if(line.isEmpty()) continue;
        QStringList p=line.split("|"); if(p.size()<13) continue;
        // Skip old-format records with auto-incremented short integer IDs
        // Valid student IDs are "ADMIN" or 10-digit numbers
        QString sid = p[0];
        bool isShortInt = false;
        sid.toInt(&isShortInt);
        if (isShortInt && sid.length() < 8) continue; // skip old numeric IDs
        auto* node=new StudentNode(p[0],p[1],p[2],p[3],p[4],p[5],p[6],p[7],
                                   p[8].toDouble(),p[9],p[11],p[12]);
        node->joinedGroups=p[10].toInt();
        sm.appendRaw(node);
    }
    return true;
}

bool FileHandler::loadGroups(GroupManager& gm) {
    QFile f(groupsFile()); if(!f.open(QIODevice::ReadOnly|QIODevice::Text)) return false;
    QTextStream in(&f);
    while (!in.atEnd()) {
        QString line=in.readLine().trimmed(); if(line.isEmpty()) continue;
        QStringList p=line.split("|");
        if (p[0]=="NEXTSEQ") { gm.setNextSeq(p[1].toInt()); continue; }
        if (p[0]=="NEXTID")  { continue; } // legacy, skip
        if (p[0]=="GROUP" && p.size()>=13) {
            // Support both old int IDs and new SG-format IDs
            QString gid = p[1];
            if (!gid.startsWith("SG")) gid = "SG" + gid.rightJustified(3,'0');
            auto* g=new GroupNode(gid,p[2],p[3],p[4],p[5],p[6],p[7].toInt(),p[9],p[10]);
            g->currentMembers=p[8].toInt(); g->status=p[11]; g->approvedEventCount=p[12].toInt();
            gm.appendRaw(g);
        } else if (p[0]=="MEMBER" && p.size()>=4) {
            QString gid = p[1];
            if (!gid.startsWith("SG")) gid = "SG" + gid.rightJustified(3,'0');
            gm.appendMemberRaw(gid, new MemberNode(p[2],p[3]));
        }
    }
    return true;
}

bool FileHandler::loadEvents(EventManager& em) {
    QFile f(eventsFile()); if(!f.open(QIODevice::ReadOnly|QIODevice::Text)) return false;
    QTextStream in(&f);
    while (!in.atEnd()) {
        QString line=in.readLine().trimmed(); if(line.isEmpty()) continue;
        QStringList p=line.split("|");
        if (p[0]=="NEXTID") { em.setNextId(p[1].toInt()); continue; }
        if (p[0]=="EVENT" && p.size()>=12) {
            auto* node=new EventNode(p[1].toInt(),p[2],p[3],p[4],p[5],p[6],p[7],p[8],p[11]);
            node->status=p[9]; node->reminderEnabled=(p[10].toInt()==1);
            em.appendRaw(node);
        } else if (p[0]=="JOINEVENT" && p.size()>=3) {
            em.joinEvent(p[1].toInt(), p[2]);
        }
    }
    return true;
}

bool FileHandler::loadTasks(TaskManager& tm) {
    QFile f(tasksFile()); if(!f.open(QIODevice::ReadOnly|QIODevice::Text)) return false;
    QTextStream in(&f);
    while (!in.atEnd()) {
        QString line=in.readLine().trimmed(); if(line.isEmpty()) continue;
        QStringList p=line.split("|");
        if (p[0]=="NEXTID") { tm.setNextId(p[1].toInt()); continue; }
        if (p.size()<8) continue;
        // Support both old (8 col) and new (9 col with assignedBy)
        if (p.size()>=9) {
            auto* node=new TaskNode(p[0].toInt(),p[1],p[2],p[3],p[4],p[5],p[7]);
            node->progressPercent=p[6].toInt(); node->status=p[8];
            tm.appendRaw(node);
        } else {
            auto* node=new TaskNode(p[0].toInt(),p[1],p[2],p[3],p[4],"","");
            node->progressPercent=p[5].toInt(); node->status=p[7];
            tm.appendRaw(node);
        }
    }
    return true;
}

void FileHandler::loadAll(StudentManager& sm, GroupManager& gm, EventManager& em, TaskManager& tm) {
    ensureDataDir(); loadStudents(sm); loadGroups(gm); loadEvents(em); loadTasks(tm);
}

bool FileHandler::backupSystem() {
    QString ts=QDateTime::currentDateTime().toString("yyyyMMdd_hhmmss");
    QString bDir=backupDir()+"/"+ts; QDir().mkpath(bDir); bool ok=true;
    for (const QString& fn:{"students.txt","groups.txt","events.txt","tasks.txt"}) {
        QFile src(dataDir+"/"+fn); if(src.exists()) ok&=src.copy(bDir+"/"+fn);
    }
    return ok;
}

bool FileHandler::restoreSystem(StudentManager& sm, GroupManager& gm, EventManager& em, TaskManager& tm) {
    QDir bd(backupDir());
    QStringList backups=bd.entryList(QDir::Dirs|QDir::NoDotAndDotDot,QDir::Time);
    if (backups.isEmpty()) return false;
    QString latest=backupDir()+"/"+backups.first();
    for (const QString& fn:{"students.txt","groups.txt","events.txt","tasks.txt"}) {
        QFile::remove(dataDir+"/"+fn); QFile::copy(latest+"/"+fn,dataDir+"/"+fn);
    }
    loadAll(sm,gm,em,tm); return true;
}

bool FileHandler::exportReportTxt(StudentManager& sm, GroupManager& gm,
    EventManager& em, TaskManager& tm, const QString& path) {
    QFile f(path); if(!f.open(QIODevice::WriteOnly|QIODevice::Text)) return false;
    QTextStream out(&f);
    out<<"========================================\n";
    out<<"  LET'S STUDY SMART — SYSTEM REPORT\n";
    out<<"  Generated: "<<QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss")<<"\n";
    out<<"========================================\n\n";
    out<<"Total Students : "<<sm.totalStudents()<<"\n";
    out<<"Total Groups   : "<<gm.totalGroups()<<"\n";
    out<<"Total Events   : "<<em.totalEvents()<<"\n";
    out<<"Upcoming Events: "<<em.upcomingCount()<<"\n";
    out<<"Total Tasks    : "<<tm.totalTasks()<<"\n";
    out<<"Tasks Completed: "<<QString::number(tm.completedPercent(),'f',1)<<"%\n\n";
    out<<"--- STUDENTS ---\n"; for(const QString& s:sm.getAllFormatted()) out<<s<<"\n";
    out<<"\n--- GROUPS ---\n"; for(const QString& g:gm.getAllFormatted()) out<<g<<"\n";
    out<<"\n--- EVENTS ---\n"; for(const QString& e:em.getAll()) out<<e<<"\n";
    out<<"\n--- PENDING TASKS ---\n"; for(const QString& t:tm.getPending()) out<<t<<"\n";
    return true;
}
