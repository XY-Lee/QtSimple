/****************************************************************************
**
** Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
** Contact: https://www.qt.io/licensing/
**
** This file is part of the Qt3D module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** BSD License Usage
** Alternatively, you may use this file under the terms of the BSD license
** as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of The Qt Company Ltd nor the names of its
**     contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include <QGuiApplication>

#include <Qt3DCore/QEntity>
#include <Qt3DRender/QCamera>
#include <Qt3DRender/QCameraLens>
#include <Qt3DCore/QTransform>
#include <Qt3DCore/QAspectEngine>

#include <Qt3DInput/QInputAspect>

#include <Qt3DRender/QRenderAspect>
#include <Qt3DExtras/QForwardRenderer>
#include <Qt3DExtras/QPhongMaterial>
#include <Qt3DExtras/QCylinderMesh>
#include <Qt3DExtras/QSphereMesh>
#include <Qt3DExtras/QTorusMesh>

#include <QPropertyAnimation>

#include "qt3dwindow.h"
#include "orbittransformcontroller.h"
#include "qorbitcameracontroller.h"

#include <Qt3DRender/QPaintedTextureImage>
#include <QPainter>
#include <QImage>
#include <QPaintDevice>
#include <Qt3DExtras/QPlaneMesh>
#include <Qt3DExtras/QTextureMaterial>
#include <Qt3DRender/QTexture>
#include <Qt3DRender/QRenderSettings>
#include <Qt3DExtras/QPhongAlphaMaterial>
#include <Qt3DExtras/QDiffuseMapMaterial>
#include <qsvgrenderer.h>

Qt3DRender::QCamera *camera ;
class FlippedTextureImage : public Qt3DRender::QPaintedTextureImage
{
public:
    FlippedTextureImage(Qt3DCore::QNode *parent = Q_NULLPTR):Qt3DRender::QPaintedTextureImage(parent) {}
    void paint(QPainter *painter) override {
        // qDebug() << backgroundImage.isNull();
        qDebug() << qApp->applicationDirPath() + "/gift.svg";
        QSvgRenderer renderer(QString(qApp->applicationDirPath() + "/gift.svg"));
        QImage image(3000, 3000, QImage::Format_RGBA8888);  // 512x512 RGBA
        image.fill(0x00ffffff);                           // white background
        QPainter painter2(&image);
        renderer.render(&painter2);
        painter->setCompositionMode(QPainter::CompositionMode::CompositionMode_Source);
        painter->drawImage(0, 0, image);
    }

private:
    // QImage backgroundImage = QImage(qApp->applicationDirPath() + "/gift.svg");
};

class planeTransformClass : public Qt3DCore::QTransform {

public:
    planeTransformClass( Qt3DCore::QNode *entity = nullptr ) :Qt3DCore::QTransform(entity) {}

signals:
    void faceTo( QVector3D v ) {
        /*
        QVector3D vv( v.x(), -v.z(), v.y() );
        setRotation(QQuaternion::rotationTo(QVector3D(0,1,0), -v));
        qDebug() << camera->upVector();
        QVector3D vup( camera->upVector().x(), -camera->upVector().z(), camera->upVector().y() );
        setRotation(QQuaternion::rotationTo(rotation().rotatedVector(QVector3D(0,0,1)), vup));
        */

        //QVector3D vup( camera->upVector().x(), -camera->upVector().z(), camera->upVector().y() );
        //setRotation(QQuaternion::rotationTo(QVector3D(0,0,1), vup));
        setRotation(QQuaternion::rotationTo(QVector3D(0,1,0), -v));
    } ;
};

Qt3DCore::QEntity *planeEntity ;
Qt3DExtras::QPlaneMesh *planeMesh ;
Qt3DExtras::QTextureMaterial *planeMaterial ;
planeTransformClass *planeTransform ;

Qt3DCore::QEntity *createScene()
{
    // Root entity
    Qt3DCore::QEntity *rootEntity = new Qt3DCore::QEntity;

    // Material
    Qt3DRender::QMaterial *material = new Qt3DExtras::QPhongMaterial(rootEntity);

    // Background
        planeEntity = new Qt3DCore::QEntity(rootEntity);
        planeMesh = new Qt3DExtras::QPlaneMesh(planeEntity);
        planeMesh->setHeight(2);
        planeMesh->setWidth(2);

        planeMaterial = new Qt3DExtras::QTextureMaterial(planeEntity);
        Qt3DRender::QTexture2D *planeTexture = new Qt3DRender::QTexture2D(planeMaterial);

        FlippedTextureImage *planeTextureImage = new FlippedTextureImage(planeTexture);
        planeTextureImage->setSize(QSize(3000, 3000));
        planeTexture->addTextureImage(planeTextureImage);
        planeMaterial->setTexture(planeTexture);
        planeMaterial->setAlphaBlendingEnabled(true);
        planeTransform = new planeTransformClass(planeEntity);
        planeTransform->setRotationX(90);
        planeTransform->setTranslation(QVector3D(2, 0, 0));

        planeEntity->addComponent(planeMesh);
        planeEntity->addComponent(planeMaterial);
        planeEntity->addComponent(planeTransform);


    // Torus
    Qt3DCore::QEntity *torusEntity = new Qt3DCore::QEntity(rootEntity);
    Qt3DExtras::QTorusMesh *torusMesh = new Qt3DExtras::QTorusMesh;
    torusMesh->setRadius(5);
    torusMesh->setMinorRadius(1);
    torusMesh->setRings(100);
    torusMesh->setSlices(20);

    Qt3DCore::QTransform *torusTransform = new Qt3DCore::QTransform;
    torusTransform->setScale3D(QVector3D(1.5, 1, 0.5));
    torusTransform->setRotation(QQuaternion::fromAxisAndAngle(QVector3D(1, 0, 0), 90.0f));

    torusEntity->addComponent(torusMesh);
    torusEntity->addComponent(torusTransform);
    torusEntity->addComponent(material);


    // Sphere
    Qt3DCore::QEntity *sphereEntity = new Qt3DCore::QEntity(rootEntity);
    Qt3DExtras::QSphereMesh *sphereMesh = new Qt3DExtras::QSphereMesh;
    sphereMesh->setRadius(3);

    Qt3DCore::QTransform *sphereTransform = new Qt3DCore::QTransform;
    sphereTransform->setTranslation(QVector3D(-3,0,0));
//    OrbitTransformController *controller = new OrbitTransformController(sphereTransform);
//    controller->setTarget(sphereTransform);
//    controller->setRadius(20.0f);

//    QPropertyAnimation *sphereRotateTransformAnimation = new QPropertyAnimation(sphereTransform);
//    sphereRotateTransformAnimation->setTargetObject(controller);
//    sphereRotateTransformAnimation->setPropertyName("angle");
//    sphereRotateTransformAnimation->setStartValue(QVariant::fromValue(0));
//    sphereRotateTransformAnimation->setEndValue(QVariant::fromValue(360));
//    sphereRotateTransformAnimation->setDuration(10000);
//    sphereRotateTransformAnimation->setLoopCount(-1);
//    sphereRotateTransformAnimation->start();

    sphereEntity->addComponent(sphereMesh);
    sphereEntity->addComponent(sphereTransform);
    sphereEntity->addComponent(material);


    return rootEntity;
}

int main(int argc, char* argv[])
{
    QGuiApplication app(argc, argv);
    Qt3DExtras::Qt3DWindow view;

    Qt3DCore::QEntity *scene = createScene();

    // Camera
    camera = view.camera();
    camera->lens()->setPerspectiveProjection(45.0f, 16.0f/9.0f, 0.1f, 1000.0f);
    camera->setPosition(QVector3D(0, 0, 40.0f));
    camera->setViewCenter(QVector3D(0, 0, 0));

    // For camera controls
    Qt3DExtras::QOrbitCameraController *camController = new Qt3DExtras::QOrbitCameraController(scene);
    camController->setLinearSpeed( 50.0f );
    camController->setLookSpeed( 180.0f );
    camController->setCamera(camera);

    QVector3D v = camera->position();
    qDebug() << v;
    qDebug() << planeTransform->worldMatrix() ;
    qDebug() << planeTransform->matrix() ;
    //planeTransform->setRotation(QQuaternion::rotationTo(QVector3D(0,1,0), v));
    qDebug() << planeTransform->worldMatrix() ;
    qDebug() << planeTransform->matrix() ;
    qDebug() << planeTransform->matrix() * QVector3D(0,1,0) ;

    camera->connect( camera, &Qt3DRender::QCamera::viewVectorChanged, planeTransform, &planeTransformClass::faceTo);

    view.setRootEntity(scene);
    view.show();

    return app.exec();
}
