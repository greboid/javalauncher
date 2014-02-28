#include "JVMLauncher.h"

JVMLauncher::JVMLauncher(std::string path, std::string mainClassName, ConfigReader* config) {
    //Get JVM.dll via JAVA_HOME
    javaHome = getenv("JAVA_HOME");
    if (javaHome.empty()) {
        throw JVMLauncherException("JAVA_HOME not defined");
    }
    jvmDll = javaHome + "\\jre\\bin\\server\\jvm.dll";
    //Do something better here..
    appHome.append(path);
    jars.push_back("DMDirc.jar");
    this->mainClassName = mainClassName;
    this->config = config;
}

HANDLE JVMLauncher::forkAndLaunch() {
    //Create thread pointing it at new thread entry point
    return CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE) threadEntry,
            (LPVOID) this, 0, NULL);
}

UINT WINAPI JVMLauncher::threadEntry(LPVOID param) {
    //cast param to object instance
    JVMLauncher* obj = (JVMLauncher*) param;
    //Call jvm launch
    obj->LaunchJVM();
}

void JVMLauncher::LaunchJVM() {
    //Build library path
    std::string strJavaLibraryPath = "-Djava.library.path=";
    strJavaLibraryPath += javaHome + "\\lib" + "," + javaHome + "\\jre\\lib";
    //Add jars to classpath
    std::string strJavaClassPath = "-Djava.class.path=";
    for (int i = 0; i < jars.size() - 1; i++) {
        strJavaClassPath += appHome + jars[i] + ";";
    }
    strJavaClassPath += appHome + jars[jars.size() - 1];
    //Configure JVM Options
    JavaVMOption options[3];
    options[0].optionString = (char*) (strJavaClassPath.c_str());
    options[1].optionString = (char*) (strJavaLibraryPath.c_str());
    options[2].optionString = (char*) "exit";
    options[2].extraInfo = (void*) *JVMLauncher::exit;
    //Configure VM args
    JavaVMInitArgs vm_args;
    vm_args.version = JNI_VERSION_1_6; //JNI Version 1.4 and above
    vm_args.options = options;
    vm_args.nOptions = 3;
    vm_args.ignoreUnrecognized = JNI_FALSE;
    //Load JVM.dll
    jvmDllInstance = LoadLibraryA(jvmDll.c_str());
    if (jvmDllInstance == 0) {
        throw JVMLauncherException("Cannot load jvm.dll");
    }
    //Load JVM
    jvmInstance = (CreateJavaVM) GetProcAddress(jvmDllInstance, "JNI_CreateJavaVM");
    if (jvmInstance == NULL) {
        throw JVMLauncherException("Cannot load jvm.dll");
    }
    //Create the JVM
    jint res = jvmInstance(&jvm, (void **) &jvmEnv, &vm_args);
    if (res < 0) {
        throw JVMLauncherException("Could not launch the JVM");
    }
    //Get main class
    mainClass = jvmEnv->FindClass((char*) mainClassName.c_str());
    checkForException();
    //Get main method
    mainMethod = jvmEnv->GetStaticMethodID(mainClass, "main", "([Ljava/lang/String;)V");
    checkForException();
    //Attach to main thread
    jvm->AttachCurrentThread((LPVOID*) & jvmEnv, NULL);
    //Get main method args
    jclass StringClass = jvmEnv->FindClass("java/lang/String");
    jobjectArray jargs = jvmEnv->NewObjectArray(2, StringClass, jvmEnv->NewStringUTF(""));
    jvmEnv->SetObjectArrayElement(jargs, 0, jvmEnv->NewStringUTF("-d"));
    jvmEnv->SetObjectArrayElement(jargs, 1, jvmEnv->NewStringUTF("c:\\dmdirc-test\\"));
    //Call main method
    jvmEnv->CallStaticVoidMethod(mainClass, mainMethod, jargs);
    jvm->DetachCurrentThread();
    jvm->DestroyJavaVM();
}

void JVMLauncher::exit(jint status) {
    cout << "JVM quitting: " << status << endl;
}

void JVMLauncher::checkForException() {
    //Check exception happened
    jthrowable ex = jvmEnv->ExceptionOccurred();
    if (ex != NULL) {
        //clear exception
        jvmEnv->ExceptionClear();
        //Grab info about exception and throw
        jmethodID toString = jvmEnv->GetMethodID(jvmEnv->FindClass("java/lang/Object"), "toString", "()Ljava/lang/String;");
        jstring estring = (jstring) jvmEnv->CallObjectMethod(ex, toString);
        jboolean isCopy;
        std::string message = jvmEnv->GetStringUTFChars(estring, &isCopy);
        throw JVMLauncherException(message);
    }
}