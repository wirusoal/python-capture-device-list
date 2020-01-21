#ifdef __cplusplus
extern "C" {
#endif
struct devices_list {
    int id;
    char * name;
    char * path;
};

struct devices_count {
    int countdevice;
    devices_list list[10];
};
__declspec(dllexport) auto devices();
#ifdef __cplusplus
}
#endif


#include <stdio.h>
#include <iostream>
#include <windows.h>
#include <dshow.h>
#include <comutil.h>

#pragma comment(lib, "comsuppwd.lib")
#pragma comment(lib, "strmiids")
#pragma comment(lib, "ole32.lib")
#pragma comment(lib, "oleaut32.lib")

HRESULT EnumerateDevices(REFGUID category, IEnumMoniker **ppEnum)
{
    // Create the System Device Enumerator.
    ICreateDevEnum *pDevEnum;
    HRESULT hr = CoCreateInstance(CLSID_SystemDeviceEnum, NULL,
        CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&pDevEnum));

    if (SUCCEEDED(hr))
    {
        // Create an enumerator for the category.
        hr = pDevEnum->CreateClassEnumerator(category, ppEnum, 0);
        if (hr == S_FALSE)
        {
            hr = VFW_E_NOT_FOUND;  // The category is empty. Treat as an error.
        }
        pDevEnum->Release();
    }
    return hr;
}


devices_count DisplayDeviceInformation(IEnumMoniker *pEnum)
{
    devices_count devices_list;
    int len = 0;
    IMoniker *pMoniker = NULL;
    char* ret = NULL;

    while (pEnum->Next(1, &pMoniker, NULL) == S_OK)
    {
        IPropertyBag *pPropBag;
        HRESULT hr = pMoniker->BindToStorage(0, 0, IID_PPV_ARGS(&pPropBag));
        if (FAILED(hr))
        {
            pMoniker->Release();
            continue;
        }

        VARIANT var;
        VariantInit(&var);

        // Get description or friendly name.
        hr = pPropBag->Read(L"Description", &var, 0);
        if (FAILED(hr))
        {
            hr = pPropBag->Read(L"FriendlyName", &var, 0);
        }
        if (SUCCEEDED(hr))
        {
            devices_list.list[len].id = len;
            char  *pValue = _com_util::ConvertBSTRToString(var.bstrVal);
            devices_list.list[len].name = pValue;
            VariantClear(&var);
        }

        hr = pPropBag->Write(L"FriendlyName", &var);

        // WaveInID applies only to audio capture devices.
        hr = pPropBag->Read(L"WaveInID", &var, 0);
        if (SUCCEEDED(hr))
        {
           // printf("WaveIn ID: %d\n", var.lVal);
            VariantClear(&var);
        }

        hr = pPropBag->Read(L"DevicePath", &var, 0);
        if (SUCCEEDED(hr))
        {
            // The device path is not intended for display.
            char  *pValue = _com_util::ConvertBSTRToString(var.bstrVal);
            devices_list.list[len].path = pValue;
            VariantClear(&var);
        }

        pPropBag->Release();
        pMoniker->Release();
        len++;
        devices_list.countdevice = len;
    }
    return devices_list;
}

auto devices() {
    HRESULT hr = CoInitializeEx(NULL, COINIT_MULTITHREADED);
    if (SUCCEEDED(hr))
    {
        IEnumMoniker *pEnum;

        hr = EnumerateDevices(CLSID_VideoInputDeviceCategory, &pEnum);
        if (SUCCEEDED(hr))
        {
            return DisplayDeviceInformation(pEnum);
            pEnum->Release();
        }
        hr = EnumerateDevices(CLSID_AudioInputDeviceCategory, &pEnum);
        if (SUCCEEDED(hr))
        {
             return DisplayDeviceInformation(pEnum);
            pEnum->Release();
        }
        CoUninitialize();
    }
}