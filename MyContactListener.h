#pragma once
#ifndef MYCONTACTLISTENER_H
#define MYCONTACTLISTENER_H

#include "reactphysics3d/reactphysics3d.h"
using namespace rp3d;
using namespace std;

enum ContactEventType {
    ContactStart,
    ContactStay,
    ContactExit
};

class MyContactListener : public EventListener {
public:

    bool isInContact = false;
    uint cristobal;
    bool band = false;

    virtual void onContact(const CollisionCallback::CallbackData& callbackData) override {
        for (uint p = 0; p < callbackData.getNbContactPairs(); p++) {
            CollisionCallback::ContactPair contactPair = callbackData.getContactPair(p);

            if (contactPair.getBody1() == cuerpo && contactPair.getBody2() == piso)
            {

                if (contactPair.getEventType() == ContactPair::EventType::ContactStart) {
                    isInContact = true;
                }

                if (contactPair.getEventType() == ContactPair::EventType::ContactStay) {
                    isInContact = true;
                }

            }
            if (contactPair.getEventType() == ContactPair::EventType::ContactExit) {
                isInContact = false;
            }
        }
    }

    // Puedes agregar referencias al cuerpo y al piso para utilizarlas en onContact
    void setBodies(RigidBody* cuerpo, RigidBody* piso) {
        this->cuerpo = cuerpo;
        this->piso = piso;
    }

private:
    RigidBody* cuerpo;
    RigidBody* piso;
};

#endif