/**
 * Copyright(c) Live2D Inc. All rights reserved.
 *
 * Use of this source code is governed by the Live2D Open Software license
 * that can be found at https://www.live2d.com/eula/live2d-open-software-license-agreement_en.html.
 */

#pragma once

#include "Framework/CubismFramework.hpp"
#include "Framework/Model/CubismUserModel.hpp"
#include "Framework/ICubismModelSetting.hpp"
#include "Framework/Type/csmRectF.hpp"

#include "LAppWavFileHandler.h"
//#include "CubismModel3JsonParser.h"

class LAppRenderer;

class LAppModel : public Csm::CubismUserModel
{
public:
    LAppModel(LAppRenderer* renderer);

    virtual ~LAppModel();

    bool LoadAssets(const Csm::csmChar* dir, const  Csm::csmChar* fileName);
    //bool LoadAssets(QString filePath);

    void Update();

    void Draw(Csm::CubismMatrix44& matrix);

    Csm::CubismMotionQueueEntryHandle StartMotion(const Csm::csmChar* group, Csm::csmInt32 no, Csm::csmInt32 priority, Csm::ACubismMotion::FinishedMotionCallback onFinishedMotionHandler = NULL);

    Csm::CubismMotionQueueEntryHandle StartRandomMotion(const Csm::csmChar* group, Csm::csmInt32 priority, Csm::ACubismMotion::FinishedMotionCallback onFinishedMotionHandler = NULL);

    void SetExpression(const Csm::csmChar* expressionID);

    void SetRandomExpression();

    virtual void MotionEventFired(const Live2D::Cubism::Framework::csmString& eventValue);

    virtual Csm::csmBool HitTest(const Csm::csmChar* hitAreaName, Csm::csmFloat32 x, Csm::csmFloat32 y);

    Csm::ICubismModelSetting* GetModelSetting() const { return _modelSetting; }
    //CubismModel3JsonParser* GetModelSetting() const { return _modelSetting; }
    void ReleaseAssets();

protected:
    void DoDraw();

private:
    bool SetupModel(Csm::ICubismModelSetting* setting);
    //bool SetupModel(Csm::CubismModel3JsonParser* setting);

    void PreloadMotionGroup(const Csm::csmChar* group);

    void ReleaseMotionGroup(const Csm::csmChar* group) const;

    void ReleaseMotions();

    void ReleaseExpressions();

    Csm::ICubismModelSetting* _modelSetting;
    //Csm::CubismModel3JsonParser* _modelSetting;
    Csm::csmString _modelHomeDir;
    //QString _modelHomeDir;
    Csm::csmFloat32 _userTimeSeconds;
    Csm::csmVector<Csm::CubismIdHandle> _eyeBlinkIds;
    Csm::csmVector<Csm::CubismIdHandle> _lipSyncIds;
    Csm::csmMap<Csm::csmString, Csm::ACubismMotion*>   _motions;
    Csm::csmMap<Csm::csmString, Csm::ACubismMotion*>   _expressions;
    Csm::csmVector<Csm::csmRectF> _hitArea;
    Csm::csmVector<Csm::csmRectF> _userArea;
    const Csm::CubismId* _idParamAngleX;
    const Csm::CubismId* _idParamAngleY;
    const Csm::CubismId* _idParamAngleZ;
    const Csm::CubismId* _idParamBodyAngleX;
    const Csm::CubismId* _idParamEyeBallX;
    const Csm::CubismId* _idParamEyeBallY;

    LAppWavFileHandler _wavFileHandler;

    LAppRenderer* _renderer;
};



