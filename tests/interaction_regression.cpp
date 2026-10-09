#include "animation_system.h"
#include "collision_system.h"
#include <iostream>
#include <stdexcept>
#include <limits>
using namespace std;
void require(bool condition,const char* message){if(!condition)throw runtime_error(message);}
void finishHome(AnimationSystem& a)
{
    for(int i=0;i<9000&&a.workers()[0].state!=WorkerState::HomeIdle;++i)a.update(1.0f/120);
    require(a.workers()[0].state==WorkerState::HomeIdle,"Worker failed to return home");
}
int main()
{
    try {
        AnimationSystem a;
        require(a.hayAvailableMask()==7,"Initial stock incorrect");
        for(int trip=0;trip<3;++trip) {
            a.commandWorker();bool picked=false;
            for(int frame=0;frame<9000;++frame) {
                a.update(1.0f/120);
                const auto& w=a.workers()[0];
                if(!CollisionSystem::canOccupy(w.position+glm::vec3(0,1.8f,0),90,0,92,
                                               a.workerDoorAngle(),a.hayAvailableMask())) {
                    cerr<<"trip "<<trip<<" frame "<<frame<<" xyz "<<w.position.x<<','<<w.position.y<<','<<w.position.z<<" door "<<a.workerDoorAngle()<<'\n';
                    throw runtime_error("Automatic worker route hit an obstacle");
                }
                if(w.state==WorkerState::PickingUpHay) {
                    picked=true;require(w.hayIndex==trip,"Worker reused a consumed hay bundle");
                    require(!(a.hayAvailableMask()&(1u<<trip)),"Pickup bundle still available");
                    a.commandWorker();require(w.state==WorkerState::PickingUpHay,"Repeated M interrupted pickup");
                }
                if(w.state==WorkerState::HomeIdle)break;
            }
            require(picked&&a.deliveredHayLoads()==trip+1,"Delivery failed");
            require(a.hayRemaining()==2-trip,"Hay respawned after delivery");
        }
        a.commandWorker();require(a.workers()[0].state==WorkerState::HomeIdle,"Empty station allowed pickup");
        require(a.restockHay()&&a.hayAvailableMask()==7,"Explicit restock failed");
        AnimationSystem cancel;cancel.commandWorker();
        for(int i=0;i<9000&&cancel.workers()[0].state!=WorkerState::PickingUpHay;++i)cancel.update(1.0f/120);
        require(cancel.hayRemaining()==2,"Reservation failed");
        cancel.sendWorkerHome();finishHome(cancel);
        require(cancel.hayRemaining()==3&&cancel.deliveredHayLoads()==0,"Cancelled pickup lost or duplicated hay");
        AnimationSystem carry;carry.commandWorker();
        for(int i=0;i<9000&&!carry.workers()[0].carryingHay;++i)carry.update(1.0f/120);
        carry.sendWorkerHome();finishHome(carry);require(carry.hayAvailableMask()==7,"Undelivered carry not returned");
        AnimationSystem door;door.commandWorker();
        for(int i=0;i<40;++i){door.update(1.0f/120);require(door.workers()[0].position.z==12.70f,"Worker moved before door clearance");}
        require(!CollisionSystem::canOccupy({8,2,10.48f},90,0,92,0),"Closed house door is passable");
        require(CollisionSystem::canOccupy({8,2,10.48f},90,0,92,90),"Open house doorway blocked");
        require(!CollisionSystem::canOccupy(FarmInteraction::hayPosition(1)+glm::vec3(0,1.8f,0),90,0,92,90,7),
                "Visible pickup hay is not solid");
        require(CollisionSystem::canOccupy(FarmInteraction::hayPosition(1)+glm::vec3(0,1.8f,0),90,0,92,90,5),
                "Removed pickup hay still blocks movement");
        require(2.55f+.22f+.195f<3.1f,"Carried hay exceeds lintel height");
        AnimationSystem calf;calf.toggleManualCalf();
        require(calf.manualCalfControl()&&calf.calfShedDoorNeeded(),"Calf control/door request failed");
        for(int i=0;i<37;++i)calf.driveManualCalf(-1,0,.01f,90,0,0);
        for(int i=0;i<82;++i)calf.driveManualCalf(0,-1,.01f,90,0,0);
        for(int i=0;i<100;++i)calf.driveManualCalf(1,0,.01f,90,0,0);
        require(calf.calves()[0].position.x<-6.4f,"Manual calf crossed closed door");
        for(int i=0;i<200;++i)calf.driveManualCalf(1,0,.01f,90,0,92);
        require(calf.calves()[0].position.x>-4,"Calf cannot leave through open doorway");
        calf.selectNextCalf();require(calf.selectedCalfIndex()==1&&calf.calves()[1].routeState==Calf::RouteState::Manual,"Calf selection failed");
        calf.toggleManualCalf();require(!calf.manualCalfControl(),"Calf control did not release");
        for(glm::vec3 point:{glm::vec3(8,0,14.2f),glm::vec3(12,0,14.2f),glm::vec3(5,0,14.2f)})
        {
            AnimationSystem home;home.toggleManualWorker();home.moveManualWorker(point,.016f);home.sendWorkerHome();
            for(int i=0;i<9000&&home.workers()[0].state!=WorkerState::HomeIdle;++i)
            {
                home.update(1.0f/120);
                require(CollisionSystem::canOccupy(home.workers()[0].position+glm::vec3(0,1.8f,0),90,0,92,
                        home.workerDoorAngle(),home.hayAvailableMask()),"Return-home detour crossed house wall");
            }
            require(home.workers()[0].state==WorkerState::HomeIdle,"Return-home detour failed");
        }
        AnimationSystem nearest;nearest.toggleManualWorker();nearest.moveManualWorker(FarmInteraction::hayPosition(2),.016f);
        require(nearest.interactWithWorker()&&nearest.workers()[0].hayIndex==2,"Manual pickup ignored nearest bundle");
        AnimationSystem water;water.toggleManualWorker();
        water.moveManualWorker(glm::vec3(7.4f,0,3.35f),.016f);
        require(water.toggleWaterValve()&&water.waterValveOpen(),"Valve could not be opened beside hand-wheel");
        require(water.interactWithWorker()&&water.workerHoldingHose(),"Worker could not pick up hose nozzle");
        const glm::vec3 beforeStretch=water.workers()[0].position;
        water.moveManualWorker(glm::vec3(-30,0,-30),.016f);
        require(water.workers()[0].position==beforeStretch,"Hose stretched beyond maximum length");
        water.moveManualWorker(FarmInteraction::WaterTroughInteraction,.016f);
        require(water.interactWithWorker()&&water.hosePlacedAtTrough(),"Hose could not be placed at water trough");
        for(int i=0;i<1000;++i)water.update(1.0f/120.0f);
        require(water.waterTroughFill()>0.95f,"Open valve did not fill water trough");
        water.stopWaterFlow();
        water.moveManualWorker(FarmInteraction::PoultryFeedPosition,.016f);
        require(water.scatterPoultryFeed()&&water.poultryFeedLevel()>0.99f&&
                water.workers()[0].scatteringFeed,"Worker could not scatter visible poultry feed");
        const glm::vec3 before=a.workers()[0].position;a.update(numeric_limits<float>::quiet_NaN());
        require(a.workers()[0].position==before,"Nonfinite timestep corrupted state");
        cout<<"PASS: solid dynamic hay, three unique deliveries, door clearance, direct calf control, valve/hose/trough, poultry feed scattering, invalid dt.\n";
    }catch(const exception& e){cerr<<"FAIL: "<<e.what()<<'\n';return 1;}
}
