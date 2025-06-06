/****************************************************************************
 Copyright (c) 2017-2018 Xiamen Yaji Software Co., Ltd.
 Copyright (c) 2019-present Axmol Engine contributors (see AUTHORS.md).

 https://axmol.dev/

 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:

 The above copyright notice and this permission notice shall be included in
 all copies or substantial portions of the Software.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 THE SOFTWARE.
 ****************************************************************************/

#include "AppDelegate.h"

#include <filesystem>
#include <physfs.h>
#include <fmt/format.h>

#include "MainScene.h"
#include "Utility.h"

#if AX_TARGET_PLATFORM == AX_PLATFORM_WIN32
    #include "platform/win32/VfsFileUtils.h"
#elif AX_TARGET_PLATFORM == AX_PLATFORM_ANDROID
#    include "platform/android/VfsFileUtils.h"

#else
    #error "Unsupported platform"
#endif

#define USE_AUDIO_ENGINE 1

#if USE_AUDIO_ENGINE
#    include "audio/AudioEngine.h"
#endif

using namespace ax;

static ax::Size designResolutionSize = ax::Size(1280, 720);

AppDelegate::AppDelegate() {}

AppDelegate::~AppDelegate() {}

// if you want a different context, modify the value of glContextAttrs
// it will affect all platforms
void AppDelegate::initGLContextAttrs()
{
    // set OpenGL context attributes: red,green,blue,alpha,depth,stencil,multisamplesCount
    GLContextAttrs glContextAttrs = {8, 8, 8, 8, 24, 8, 0};
    // since axmol-2.2 vsync was enabled in engine by default
    // glContextAttrs.vsync = false;

    GLView::setGLContextAttrs(glContextAttrs);
}

bool AppDelegate::applicationDidFinishLaunching()
{
    // initialize director
    auto director = Director::getInstance();
    auto glView   = director->getGLView();
    if (!glView)
    {
#if (AX_TARGET_PLATFORM == AX_PLATFORM_WIN32) || (AX_TARGET_PLATFORM == AX_PLATFORM_MAC) || \
    (AX_TARGET_PLATFORM == AX_PLATFORM_LINUX)
        glView = GLViewImpl::createWithRect(
            "vfs-example", ax::Rect(0, 0, designResolutionSize.width, designResolutionSize.height));
#else
        glView = GLViewImpl::create("vfs-example");
#endif
        director->setGLView(glView);
    }

    // turn on display FPS
    director->setStatsDisplay(true);

    // set FPS. the default value is 1.0/60 if you don't call this
    director->setAnimationInterval(1.0f / 60);

    // Set the design resolution
    glView->setDesignResolutionSize(designResolutionSize.width, designResolutionSize.height,
                                    ResolutionPolicy::SHOW_ALL);

    // Create VFS FileUtils based on PhysFS and set it as the default
    auto* vfsFileUtils = myapp::VfsFileUtils::create();
    FileUtils::setDelegate(vfsFileUtils);

    // This is a special case, where you need to mount a zip file that exists within the "Content" folder (so not in the writeable paths).
    // Take care with Android that you do NOT compress any ZIP files included in the APK. Read the comment of PHYSFS_mountHandle for more info.
    PHYSFS_mountHandle(PHYSFS_openRead("archive1.zip"), "archive1.zip", "", true);

    auto* fileUtils = FileUtils::getInstance();
    std::string sourceFile = "sample1.zip";
    std::string outputPath = "dlc/";
    const auto storagePath = fmt::format("{}/{}", fileUtils->getWritablePath(), outputPath);
    if (!fileUtils->isDirectoryExist(storagePath))
    {
        if (!fileUtils->createDirectory(storagePath))
        {
            AXLOG("Cannot create destination directory %s", std::string(storagePath).c_str());
        }
    }
    const auto destFile = fmt::format("{}/{}", storagePath, std::filesystem::path(sourceFile).filename().string());
    auto dstFileExists = fileUtils->isFileExist(destFile);
    if (dstFileExists)
    {
        dstFileExists = !fileUtils->removeFile(destFile);
    }

    if (!dstFileExists)
    {
        const auto fullSourceFilePath = fileUtils->fullPathForFilename(sourceFile);
        myapp::Utility::copyFile(fullSourceFilePath, destFile);
    }

    // And this is where you mount items in the writeable folder
    std::string sampleAsset = "dlc/sample1.zip";
    const auto realDir = PHYSFS_getRealDir(sampleAsset.c_str()); // Get the real directory for it (native path)
    PHYSFS_mount(fmt::format("{}{}", realDir, sampleAsset).c_str(), "dlc/mystuff/", false);

    // create a scene. it's an autorelease object
    auto scene = utils::createInstance<MainScene>();

    // run
    director->runWithScene(scene);

    return true;
}

// This function will be called when the app is inactive. Note, when receiving a phone call it is invoked.
void AppDelegate::applicationDidEnterBackground()
{
    Director::getInstance()->stopAnimation();

#if USE_AUDIO_ENGINE
    AudioEngine::pauseAll();
#endif
}

// this function will be called when the app is active again
void AppDelegate::applicationWillEnterForeground()
{
    Director::getInstance()->startAnimation();

#if USE_AUDIO_ENGINE
    AudioEngine::resumeAll();
#endif
}
