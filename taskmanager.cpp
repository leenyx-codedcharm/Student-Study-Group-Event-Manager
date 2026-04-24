#include "taskmanager.h"

TaskManager::~TaskManager() { while(head){auto* t=head;head=head->next;delete t;} }

bool TaskManager::createTask(QString groupId, QString title, QString deadline,
    QString assignedMemberId, QString assignedBy, QString notes) {
    auto* t=new TaskNode(nextId++,groupId,title,deadline,assignedMemberId,assignedBy,notes);
    if (!head){head=t;return true;}
    auto* c=head; while(c->next) c=c->next; c->next=t; return true;
}

TaskNode* TaskManager::findById(int id) {
    for(auto* t=head;t;t=t->next) if(t->id==id) return t; return nullptr;
}

bool TaskManager::updateProgress(int id, int p) {
    auto* t=findById(id); if(!t) return false;
    t->progressPercent=qBound(0,p,100);
    if(t->progressPercent==100) t->status="Completed";
    else if(t->progressPercent>0) t->status="InProgress";
    return true;
}

bool TaskManager::markCompleted(int id) {
    auto* t=findById(id); if(!t) return false;
    t->status="Completed"; t->progressPercent=100; return true;
}

bool TaskManager::deleteTask(int id) {
    if(!head) return false;
    if(head->id==id){auto* t=head;head=head->next;delete t;return true;}
    for(auto* c=head;c->next;c=c->next)
        if(c->next->id==id){auto* t=c->next;c->next=t->next;delete t;return true;}
    return false;
}

QStringList TaskManager::getByGroup(const QString& gid) {
    QStringList l;
    for(auto* t=head;t;t=t->next) if(t->groupId==gid)
        l << ("[T" + QString::number(t->id) + "] " + t->title +
              " | Due:" + t->deadline + " | Assigned:" + t->assignedMemberId +
              " | " + QString::number(t->progressPercent) + "% | " + t->status);
    return l;
}

QStringList TaskManager::getByGroupDetailed(const QString& gid) {
    QStringList l;
    for(auto* t=head;t;t=t->next) if(t->groupId==gid) {
        l << QString("━━ [T%1] %2").arg(t->id).arg(t->title);
        l << QString("   Group:%1 | Deadline:%2 | Assigned to:%3 | By:%4")
             .arg(t->groupId).arg(t->deadline).arg(t->assignedMemberId).arg(t->assignedBy);
        l << QString("   Progress:%1% | Status:%2 | Notes:%3")
             .arg(t->progressPercent).arg(t->status).arg(t->notes);
    }
    return l;
}

QStringList TaskManager::getByMember(const QString& mid) {
    QStringList l;
    for(auto* t=head;t;t=t->next) if(t->assignedMemberId==mid)
        l << ("[T" + QString::number(t->id) + "] " + t->title +
              " | Group:" + t->groupId + " | Due:" + t->deadline +
              " | " + QString::number(t->progressPercent) + "% | " + t->status);
    return l;
}

QStringList TaskManager::getPending() {
    QStringList l;
    for(auto* t=head;t;t=t->next) if(t->status!="Completed")
        l << ("[T" + QString::number(t->id) + "] " + t->title +
              " | Group:" + t->groupId + " | Due:" + t->deadline +
              " | " + QString::number(t->progressPercent) + "% | " + t->status);
    return l;
}

QStringList TaskManager::getAll() {
    QStringList l;
    for(auto* t=head;t;t=t->next)
        l << ("[T" + QString::number(t->id) + "] " + t->title +
              " | Group:" + t->groupId + " | Due:" + t->deadline +
              " | Assigned:" + t->assignedMemberId +
              " | " + QString::number(t->progressPercent) + "% | " + t->status);
    return l;
}

QStringList TaskManager::getAllDetailed() {
    QStringList l;
    for(auto* t=head;t;t=t->next) {
        l << QString("━━ [T%1] %2").arg(t->id).arg(t->title);
        l << QString("   Group:%1 | Deadline:%2 | Assigned to:%3 | By:%4")
             .arg(t->groupId).arg(t->deadline).arg(t->assignedMemberId).arg(t->assignedBy);
        l << QString("   Progress:%1% | Status:%2 | Notes:%3")
             .arg(t->progressPercent).arg(t->status).arg(t->notes);
    }
    return l;
}

int TaskManager::totalTasks()    { int n=0; for(auto* t=head;t;t=t->next) n++; return n; }
int TaskManager::completedCount(){ int n=0; for(auto* t=head;t;t=t->next) if(t->status=="Completed") n++; return n; }
double TaskManager::completedPercent() { int tot=totalTasks(); return tot?completedCount()*100.0/tot:0.0; }

void TaskManager::sortByDeadline() {
    if(!head) return; bool sw;
    do { sw=false;
        for(auto* t=head;t->next;t=t->next) if(t->deadline>t->next->deadline){
            std::swap(t->id,t->next->id); std::swap(t->groupId,t->next->groupId);
            std::swap(t->title,t->next->title); std::swap(t->deadline,t->next->deadline);
            std::swap(t->assignedMemberId,t->next->assignedMemberId);
            std::swap(t->assignedBy,t->next->assignedBy);
            std::swap(t->progressPercent,t->next->progressPercent);
            std::swap(t->notes,t->next->notes); std::swap(t->status,t->next->status);
            sw=true;
        }
    } while(sw);
}

void TaskManager::appendRaw(TaskNode* node) {
    if(!head) head=node; else{auto* t=head;while(t->next)t=t->next;t->next=node;}
    if(node->id>=nextId) nextId=node->id+1;
}
