/**
 * Copyright(c) Live2D Inc. All rights reserved.
 *
 * Use of this source code is governed by the Live2D Open Software license
 * that can be found at https://www.live2d.com/eula/live2d-open-software-license-agreement_en.html.
 */

#pragma once

#include "CubismSDK/Framework/CubismFramework.hpp"
#include "CubismSDK/Framework/Model/CubismUserModel.hpp"
#include "CubismSDK/Framework/ICubismModelSetting.hpp"
#include "CubismSDK/Framework/Type/csmRectF.hpp"

#include "LAppWavFileHandler.h"
#include <QObject>
#include <QAudio>
#include <QFile>

class QAudioOutput;

 /**
  * @brief ��`���`���g�H��ʹ�ä����ǥ�Όgװ���饹<br>
  *         ��ǥ����ɡ��C�ܥ���ݩ`�ͥ�����ɡ����I��ȥ�����󥰤κ��ӳ������Ф���
  *
  */
class LAppModel : public QObject, public Csm::CubismUserModel
{
    Q_OBJECT
public:
    /**
     * @brief ���󥹥ȥ饯��
     */
    LAppModel();

    /**
     * @brief �ǥ��ȥ饯��
     *
     */
    virtual ~LAppModel();

    /**
     * @brief model3.json���ä��줿�ǥ��쥯�ȥ�ȥե�����ѥ������ǥ�����ɤ���
     *
     */
    bool LoadAssets(const Csm::csmChar* dir, const  Csm::csmChar* fileName);

    /**
     * @brief   ��ǥ�θ��I����ǥ�Υѥ��`�������軭״�B��Q�����롣
     *
     */
    void Update();

    /**
     * @brief   ��ǥ���軭����I����ǥ���軭������g��View-Projection���Ф�ɤ���
     *
     * @param[in]  matrix  View-Projection����
     */
    void Draw(Csm::CubismMatrix44& matrix);

    /**
     * @brief   ������ָ��������`�������������_ʼ���롣
     *
     * @param[in]   group                       ��`����󥰥�`����
     * @param[in]   no                          ����`���ڤη���
     * @param[in]   priority                    ���ȶ�
     * @param[in]   onFinishedMotionHandler     ��`����������K�˕r�˺��ӳ�����륳�`��Хå��v����NULL�Έ��ϡ����ӳ�����ʤ���
     * @return                                  �_ʼ������`�������R�e���Ť򷵤������e�Υ�`����󤬽K�ˤ������񤫤��ж�����IsFinished()��������ʹ�ä��롣�_ʼ�Ǥ��ʤ��r�ϡ�-1��
     */
    Csm::CubismMotionQueueEntryHandle StartMotion(const Csm::csmChar* group, Csm::csmInt32 no, Csm::csmInt32 priority, Csm::ACubismMotion::FinishedMotionCallback onFinishedMotionHandler = NULL);

    /**
     * @brief   ��������x�Ф줿��`�������������_ʼ���롣
     *
     * @param[in]   group                       ��`����󥰥�`����
     * @param[in]   priority                    ���ȶ�
     * @param[in]   onFinishedMotionHandler     ��`����������K�˕r�˺��ӳ�����륳�`��Хå��v����NULL�Έ��ϡ����ӳ�����ʤ���
     * @return                                  �_ʼ������`�������R�e���Ť򷵤������e�Υ�`����󤬽K�ˤ������񤫤��ж�����IsFinished()��������ʹ�ä��롣�_ʼ�Ǥ��ʤ��r�ϡ�-1��
     */
    Csm::CubismMotionQueueEntryHandle StartRandomMotion(const Csm::csmChar* group, Csm::csmInt32 priority, Csm::ACubismMotion::FinishedMotionCallback onFinishedMotionHandler = NULL);

    /**
     * @brief   ������ָ�����������`�����򥻥åȤ���
     *
     * @param   expressionID    �����`������ID
     */
    void SetExpression(const Csm::csmChar* expressionID);

    /**
     * @brief   ��������x�Ф줿�����`�����򥻥åȤ���
     *
     */
    void SetRandomExpression();

    /**
    * @brief   ���٥�Ȥΰk����ܤ�ȡ��
    *
    */
    virtual void MotionEventFired(const Live2D::Cubism::Framework::csmString& eventValue);

    /**
     * @brief    �������ж��ƥ��ȡ�<br>
     *            ָ��ID��픵�ꥹ�Ȥ�����Τ�Ӌ�㤷�����ˤ����ι����ڤ��ж����롣
     *
     * @param[in]   hitAreaName     �������ж���ƥ��Ȥ��댝���ID
     * @param[in]   x               �ж����Ф�X����
     * @param[in]   y               �ж����Ф�Y����
     */
    virtual Csm::csmBool HitTest(const Csm::csmChar* hitAreaName, Csm::csmFloat32 x, Csm::csmFloat32 y);

    Csm::ICubismModelSetting* GetModelSetting() const { return _modelSetting; }

protected:
    /**
     *  @brief  ��ǥ���軭����I����ǥ���軭������g��View-Projection���Ф�ɤ���
     *
     */
    void DoDraw();

private slots:
    void handleVoiceStateChanged(QAudio::State newState);

private:
    /**
     * @brief model3.json�����ǥ�����ɤ��롣<br>
     *         model3.json��ӛ���ˏ��äƥ�ǥ����ɡ���`�������������ʤɤΥ���ݩ`�ͥ�����ɤ��Ф���
     *
     * @param[in]   setting     ICubismModelSetting�Υ��󥹥���
     *
     */
    bool SetupModel(Csm::ICubismModelSetting* setting);

    /**
     * @brief   ��`�����ǩ`���򥰥�`��������һ���ǥ�`�ɤ��롣<br>
     *           ��`�����ǩ`������ǰ���ڲ���ModelSetting����ȡ�ä��롣
     *
     * @param[in]   group  ��`�����ǩ`���Υ���`����
     */
    void PreloadMotionGroup(const Csm::csmChar* group);

    /**
     * @brief   ��`�����ǩ`���򥰥�`��������һ���ǽ�Ť��롣<br>
     *           ��`�����ǩ`������ǰ���ڲ���ModelSetting����ȡ�ä��롣
     *
     * @param[in]   group  ��`�����ǩ`���Υ���`����
     */
    void ReleaseMotionGroup(const Csm::csmChar* group) const;

    /**
    * @brief ���٤ƤΥ�`�����ǩ`���ν��
    *
    * ���٤ƤΥ�`�����ǩ`�����Ť��롣
    */
    void ReleaseMotions();

    /**
    * @brief ���٤Ƥα���ǩ`���ν��
    *
    * ���٤Ƥα���ǩ`�����Ť��롣
    */
    void ReleaseExpressions();

    Csm::ICubismModelSetting* _modelSetting; ///< ��ǥ륻�åƥ������
    Csm::csmString _modelHomeDir; ///< ��ǥ륻�åƥ��󥰤��ä��줿�ǥ��쥯�ȥ�
    Csm::csmFloat32 _userTimeSeconds; ///< �ǥ륿�r�g�ηe�ゎ[��]
    Csm::csmVector<Csm::CubismIdHandle> _eyeBlinkIds; ///< ��ǥ���O�����줿�ޤФ����C���åѥ��`��ID
    Csm::csmVector<Csm::CubismIdHandle> _lipSyncIds; ///< ��ǥ���O�����줿��åץ��󥯙C���åѥ��`��ID
    Csm::csmMap<Csm::csmString, Csm::ACubismMotion*>   _motions; ///< �i���z�ޤ�Ƥ����`�����Υꥹ��
    Csm::csmMap<Csm::csmString, Csm::ACubismMotion*>   _expressions; ///< �i���z�ޤ�Ƥ������Υꥹ��
    Csm::csmVector<Csm::csmRectF> _hitArea;
    Csm::csmVector<Csm::csmRectF> _userArea;
    const Csm::CubismId* _idParamAngleX; ///< �ѥ��`��ID: ParamAngleX
    const Csm::CubismId* _idParamAngleY; ///< �ѥ��`��ID: ParamAngleX
    const Csm::CubismId* _idParamAngleZ; ///< �ѥ��`��ID: ParamAngleX
    const Csm::CubismId* _idParamBodyAngleX; ///< �ѥ��`��ID: ParamBodyAngleX
    const Csm::CubismId* _idParamEyeBallX; ///< �ѥ��`��ID: ParamEyeBallX
    const Csm::CubismId* _idParamEyeBallY; ///< �ѥ��`��ID: ParamEyeBallXY

    LAppWavFileHandler _wavFileHandler; ///< wav�ե�����ϥ�ɥ�

    QAudioOutput* _audioOutput;
    QFile _audioFile;
};



