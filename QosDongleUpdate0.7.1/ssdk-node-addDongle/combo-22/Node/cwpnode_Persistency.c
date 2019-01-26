/** \file

$Id: cwpnode_Persistency.c $

Copyright © 2018 Signify
All Rights Reserved.

This source code and any compilation or derivative thereof is the proprietary
information of Signify and is confidential in nature.
Under no circumstances is this software to be combined with any
Open Source Software in any way or placed under an Open Source License
of any type without the express written permission of Signify
*/

/******************************************************************************
* LOCAL INCLUDE FILES
******************************************************************************/

#include "cwpnode.h"
#include "cwpnode_Lsb.h"
#include "cwpnode_Local.h"
#include "cwpnode_Persistency.h"
#include "persistency_Cfg.h"

/******************************************************************************
* EXTERNAL INCLUDE FILES
******************************************************************************/

#include "fileio_Init.h"
#include "lcp_assert.h"
#include "log.h"
#ifdef HASEEPROM
#include "memorybank.h"
#endif
#include "BuildInfo.h"

#include "persistency_FileSerializer.h"
#include "string.h"
#ifdef HASEEPROM
#include "vardiv_memorybank.h"
#endif
#include "zigbee_lightcontrolserver.h"
#include "zigbee_ballastconfigurationserver_Cfg.h"
#include "zigbee_reportingserver.h"

#include "xnvdrive.h"
#include "zigbee_lcddr_cluster.h"
#include "zigbee_commissioningattrserver.h"

/******************************************************************************
* LOCAL MACROS AND DEFINITIONS
******************************************************************************/

//#define DEBUG_DDR_PERSISTENCY

#define NUMBER_OF_PERSISTENT_ATTRIBUTES  (sizeof(s_attributesMap) / sizeof(s_attributesMap[0]))

/******************************************************************************
* LOCAL TYPES
******************************************************************************/

/*!
 * @brief Generic attribute structute to used for persistency
*/
typedef struct cwpnode_persistency_attributes_map_t
{
    serialization_ClassType_t               attributeType;
    persistency_FileDescription_t const *   pFileDescription;
    serialization_Serializable_t const *    pAttributeSerializable;
}cwpnode_persistency_attributes_map_t;

/******************************************************************************
* LOCAL FUNCTIONS
******************************************************************************/

/******************************************************************************
* LSB FUNCTION IMPLEMENTATIONS
******************************************************************************/

/******************************************************************************
* LOCAL CONSTANTS
******************************************************************************/

static const cwpnode_persistency_attributes_map_t s_attributesMap[] =
{
    {serialization_ClassType_LvlCtrl_OnLevel,           &g_persistency_CfgLevelControlConf_fileSystem,     &zigbee_LevelCntrlServer_serializableOnLevel},
    {serialization_ClassType_LvlCtrl_OnTransTime,       &g_persistency_CfgLevelControlConf_fileSystem,     &zigbee_LevelCntrlServer_serializableOnTransTime},
    {serialization_ClassType_LvlCtrl_OffTransTime,      &g_persistency_CfgLevelControlConf_fileSystem,     &zigbee_LevelCntrlServer_serializableOffTransTime},
    {serialization_ClassType_LvlCtrl_OnOffTransTime,    &g_persistency_CfgLevelControlConf_fileSystem,     &zigbee_LevelCntrlServer_serializableOnOffTransTime},
    {serialization_ClassType_LvlCtrl_StartupLevel,      &g_persistency_CfgLevelControlConf_fileSystem,     &zigbee_lightCtrlServer_serializableStartupCurrentLevel},
    {serialization_ClassType_LevelControl,              &g_persistency_CfgLevelControlConf_fileSystem,     &zigbee_LevelCntrlServer_serializableAllAttributes},
    {serialization_ClassType_ONOFF_StartupOnoff,        &g_persistency_CfgLevelControlConf_fileSystem,     &zigbee_lightCtrlServer_serializableStartuponoff},

    {serialization_ClassType_BlstCfg_BlstStatus,        &g_persistency_CfgBallastConfigConf_fileSystem,    &zigbee_BallastConfigServer_serializableBallastStatus},
    {serialization_ClassType_BlstCfg_MinLevel,          &g_persistency_CfgBallastConfigConf_fileSystem,    &zigbee_BallastConfigServer_serializableMinLevel},
    {serialization_ClassType_BlstCfg_MaxLevel,          &g_persistency_CfgBallastConfigConf_fileSystem,    &zigbee_BallastConfigServer_serializableMaxLevel},
    {serialization_ClassType_BallastConfiguration,      &g_persistency_CfgBallastConfigConf_fileSystem,    &zigbee_BallastConfigServer_serializableAllAttributes},

    {serialization_ClassType_ReportingServer_Param,     &g_persistency_CfgReportingServer_fileSystem,      &zigbee_ReportingServer_serializableAllAttributes},
    {serialization_ClassType_ReportingServer_Erase,     &g_persistency_CfgReportingServer_fileSystem,      &zigbee_ReportingServer_serializableAllAttributes},

    {serialization_ClassType_DdrCfg,                    &g_persistency_CfgDdrConf_fileSystem,              &zigbee_lcddr_cluster_serializableAllAttributes},

    {serialization_ClassType_CommParams_Rebroadcasting, &g_persistency_CfgRebroadcasting_fileSystem,       &zigbee_commissioningattrserver_serializableRBcast},
};

/******************************************************************************
* LOCAL VARIABLES
******************************************************************************/

/*!
 * @brief The persistency queue handle
 */
static osa_QueueHandle_t s_PersistencyQueueHandle;

/*!
 * @brief Light status configuration data
 */
static cwpnode_persistencyLightStatusData_t s_sLightStatusConfData;

bool c4mnode_printDdrConfigInfo = false;
/******************************************************************************
* MAIN EVENT HANDLER
******************************************************************************/

/******************************************************************************
* EXTERNAL FUNCTIONS
******************************************************************************/
extern uint16_t halGetExtendedResetInfo(void);

/******************************************************************************
* LOCAL FUNCTIONS
******************************************************************************/

#ifdef DEBUG_DDR_PERSISTENCY
static void dumpDdrConf(filedefinition_DdrConf_Data_t *p)
{
  if (c4mnode_printDdrConfigInfo)
  {
    log_Printfbasic("internalALSsensor =    %d", p->internalALSsensor    );
    log_Printfbasic("interruptFadeAllowed = %d", p->interruptFadeAllowed );
    log_Printfbasic("ddrAlgorithm =         %d", p->ddrAlgorithm         );
    log_Printfbasic("setpointPerc =         %d", p->setpointPerc         );
    log_Printfbasic("calibPoint =           %d", p->calibPoint           );
    log_Printfbasic("deadbandLow =          %d", p->deadbandLow          );
    log_Printfbasic("deadbandHigh =         %d", p->deadbandHigh         );
    log_Printfbasic("ddrFadeUpTime =        %d", p->ddrFadeUpTime        );
    log_Printfbasic("ddrFadeDnTime =        %d", p->ddrFadeDnTime        );
    log_Printfbasic("attenuationDimUp =     %d", p->attenuationDimUp     );
    log_Printfbasic("attenuationDimDn =     %d", p->attenuationDimDn     );
    log_Printfbasic("maxStepDimUp =         %d", p->maxStepDimUp         );
    log_Printfbasic("maxStepDimDn =         %d", p->maxStepDimDn         );
    log_Printfbasic("minStep =              %d", p->minStep              );
    log_Printfbasic("minDimPerc =           %d", p->minDimPerc           );
    log_Printfbasic("maxDimPerc =           %d", p->maxDimPerc           );
    log_Printfbasic("z =                    %d", p->z                    );
    log_Printfbasic("offsetEstimate =       %d", p->offsetEstimate       );
    log_Printfbasic("reserved_1 =           %d", p->reserved_1           );
  }
}
#endif

//this function need to be cleanup future
static bool_t lookupFileIndex(const persistency_FileDescription_t* pFile, const serialization_Serializable_t* pSerializable, uint16_t* pIndex)
{
    uint8_t i;
    bool_t result = false;

    for (i = 0; i < pFile->pData->nrOfElements; i++)
    {
        if (pFile->pData->pElements[i].classType == pSerializable->pCd->classType)
        {
            *pIndex = pFile->pData->pElements[i].offset;
            result = true;
            break;
        }
    }
    return result;
}

/*!
 * @brief Search the attribute map
 */
static cwpnode_persistency_attributes_map_t const * getAttributeMap(serialization_ClassType_t attributeType)
{
    uint32_t i;
    cwpnode_persistency_attributes_map_t const * pAttributeMap = NULL;

    for(i=0; i< NUMBER_OF_PERSISTENT_ATTRIBUTES ; i++ )
    {
        if(attributeType == s_attributesMap[i].attributeType)
        {
            pAttributeMap = &s_attributesMap[i];
            break;
        }
    }

    return pAttributeMap;
}

/*!
 * @brief Serialize the attribute
 */
static bool_t serializeAttrib(serialization_ClassType_t attributeType)
{
    bool_t result = false;

    cwpnode_persistency_attributes_map_t const * pAttributeMap = getAttributeMap(attributeType);
    if(pAttributeMap != NULL)
    {
        result = persistency_FileSerializerSerialize(pAttributeMap->pFileDescription, pAttributeMap->pAttributeSerializable);
    }

    return result;
}

/*!
 * @brief Deserialize the attribute
 */
static bool_t deserializeAttrib(serialization_ClassType_t attributeType)
{
    bool_t result = false;

    cwpnode_persistency_attributes_map_t const * pAttributeMap = getAttributeMap(attributeType);
    if(pAttributeMap != NULL)
    {
        result = persistency_FileSerializerDeserialize(pAttributeMap->pFileDescription, pAttributeMap->pAttributeSerializable);
    }

    return result;
}


static bool_t serializeIndexAttribute(serialization_ClassType_t attributeType, uint8_t index)
{
    bool_t result = false;
    
    cwpnode_persistency_attributes_map_t const* pAttributeMap = getAttributeMap(attributeType);
    if(pAttributeMap != NULL)
    {    
        const persistency_FileDescription_t* pFile=pAttributeMap->pFileDescription;
        const serialization_Serializable_t* pSerializable=pAttributeMap->pAttributeSerializable;
        uint16_t offset;
        uint8_t size; 
        
        if(lookupFileIndex(pFile, pSerializable, &offset))
        {
            size = serialization_PackedObjectSize(pSerializable);
            offset += (uint16_t)(index*size);
        }

        result = persistency_FileSerializerSerializeSimpleIndex(pFile->pName, pAttributeMap->pAttributeSerializable, offset, index);
    }

    return result;
}

/*!
 * @brief Function to add one record to report.conf file
 * @param[in] attributeType    ref to serialization_ClassType_ReportingServer_Erase
 * @param[in] index            the record index, 1 based
 */
static bool_t deserializeIndexAttribute(serialization_ClassType_t attributeType, uint8_t index)
{
    bool_t result = false;


    cwpnode_persistency_attributes_map_t const* pAttributeMap = getAttributeMap(attributeType);
    if(pAttributeMap != NULL)
    {
        const persistency_FileDescription_t* pFile=pAttributeMap->pFileDescription;
        const serialization_Serializable_t* pSerializable=pAttributeMap->pAttributeSerializable;
        uint16_t offset;
        uint8_t size;

        if(lookupFileIndex(pFile, pSerializable, &offset))
        {
            size = serialization_PackedObjectSize(pSerializable);
            offset += (uint16_t)(index*size);
        }

        result = persistency_FileSerializerDeserializeSimpleIndex(pFile->pName, pAttributeMap->pAttributeSerializable, offset, index);
    }

    return result;
}

/*!
 * @brief Restore all the persistent level control attributes. To be done every bootup
 */
static void restoreLevelControlAttributes( void )
{
    bool_t result = false;
    
    /* TODO: To optimize bootup time, we may need to read the level control file at one shot */
    result = deserializeAttrib(serialization_ClassType_LevelControl);
    if(result == false)     //File Error, could be because of the file not found
    {
        (void)filedefs_LightControl_Serialize_Header();
        serializeAttrib(serialization_ClassType_LevelControl);
    }
}

static void restoreStartupAttributes( void )
{
    bool_t result = false;
    
    /* TODO: To optimize bootup time, we may need to read the level control file at one shot */
    result = deserializeAttrib(serialization_ClassType_LvlCtrl_StartupLevel);
    if(result == false)     //File Error, could be becase of the file not found
    {
        (void)filedefs_LightControl_Serialize_Header();
        serializeAttrib(serialization_ClassType_LvlCtrl_StartupLevel);
    }

    result = deserializeAttrib(serialization_ClassType_ONOFF_StartupOnoff);
    if(result == false)     //File Error, could be becase of the file not found
    {
        (void)filedefs_LightControl_Serialize_Header();
        serializeAttrib(serialization_ClassType_ONOFF_StartupOnoff);
    }
}

/*!
 * @brief Restore all the persistent Ballast control attributes. To be done every bootup
 */
static void restoreBallastConfigurationAttributes( void )
{
    bool_t result = false;

    /* TODO: To optimize bootup time, we may need to read the Ballast Configuration file at one shot */
    result = deserializeAttrib(serialization_ClassType_BallastConfiguration);
    if(result == false)     //File Error, could be because of the file not found
    {
        (void)filedefs_BallastControl_Serialize_Header();
        serializeAttrib(serialization_ClassType_BallastConfiguration);
    }
}

#if 0
/*!
 * @brief Restore Lamp metrics. To be done every bootup
 */
static void restoreLampMetrics( void )
{
    bool_t result = false;
    
    /* TODO: To optimize bootup time, we may need to read the Ballast Configuration file at one shot */
    result = deserializeAttrib(serialization_ClassType_lamp);
    if(result == false)     //File Error, could be because of the file not found
    {
        (void)filedefs_LampRun_Serialize_Header();
        serializeAttrib(serialization_ClassType_lamp);
    }
}
#endif

static void restoreDdrConfData(void)
{
    bool_t result = deserializeAttrib(serialization_ClassType_DdrCfg);
    if (result == false)     //File Error, could be because of the file not found
    {
        (void)filedefs_DaylightRegulation_Serialize_Header();
        serializeAttrib(serialization_ClassType_DdrCfg);
    }
#ifdef DEBUG_DDR_PERSISTENCY
    dumpDdrConf(&zigbee_lcddr_cluster_attribute_config);
#endif
}

static void restoreRebroadcastingData(void)
{
    bool_t result = deserializeAttrib(serialization_ClassType_CommParams_Rebroadcasting);
    if (result == false)     //File Error, could be because of the file not found
    {
        (void)filedefs_Rebroadcasting_Serialize_Header();
        serializeAttrib(serialization_ClassType_CommParams_Rebroadcasting);
    }
}

/*********** attribute reporting server *****************/
static bool_t checkFileHeader(serialization_ClassType_t attributeType)
{
    bool_t result = false;
    cwpnode_persistency_attributes_map_t const* pAttributeMap = getAttributeMap(attributeType);
    if(pAttributeMap != NULL)
    {
        const persistency_FileDescription_t* pFile=pAttributeMap->pFileDescription;
        const serialization_Serializable_t* pSerializable=pAttributeMap->pAttributeSerializable;
        if ((pFile->pName != NULL) && (pSerializable->pfSetValue != NULL) && (pSerializable->pCd != NULL))
        {
            void *pFileHandle = fileio_Open(pFile->pName, fileio_Mode_Read);
            if (pFileHandle != NULL)
            {
                fileio_Close(pFileHandle);
                result = true;
            }
        }
    }
	
    return result;
}

/*!
 * @brief Restore reporting.conf. To be done every bootup
 */
static void restoreReportingServer( void )
{
    bool_t result = false;
    uint8_t index = 0;

    result = checkFileHeader(serialization_ClassType_ReportingServer_Param);
    if(result == false)
    {   /* File not Found. Serialise the Header */
        (void)persistency_FileSerializerSerializeHeader(&g_persistency_CfgReportingServer_fileSystem, NULL);

        (void)zigbee_reportingserver_ConfigureDefaultReportAttributes();
    }
    else
    {   /*File Found, Deserialise the Data */
        for(index = 0; index < MAX_CONFIGURE_REPORTING; index++)
        {
           (void)deserializeIndexAttribute(serialization_ClassType_ReportingServer_Param, index);
        }
    }
}

/*!
 * @brief Function to remove record(s) from report.conf file
 * @param[in] attributeType    ref to serialization_ClassType_ReportingServer_Erase
 * @param[in] index            the record index, 0 based, 0xff - for erase all records
 */
static bool_t removeReportingServerAttribute(serialization_ClassType_t attributeType, uint8_t index)
{
    bool_t result = false;
    bool_t resultoffset = false;
    uint16_t offset = 0;
    uint16_t size = 0;

    cwpnode_persistency_attributes_map_t const* pAttributeMap = getAttributeMap(attributeType);
    if(pAttributeMap != NULL)
    {
          const persistency_FileDescription_t* pFile=pAttributeMap->pFileDescription;
          const serialization_Serializable_t* pSerializable=pAttributeMap->pAttributeSerializable;

          if (pFile != NULL && pSerializable != NULL)
          {
              if ((pFile->pName != NULL) && (pSerializable->pCd != NULL))
              {
            	  uint16_t i;
                  for (i = 0; ((i < pFile->pData->nrOfElements) && (resultoffset == false)); i++)
                  {
                      if (pFile->pData->pElements[i].classType == pSerializable->pCd->classType)
                      {
                          offset = pFile->pData->pElements[i].offset;
                          resultoffset = true;
                      }
                  }

                  void *pFileHandle = fileio_Open(pFile->pName, fileio_Mode_Write);
                  if (pFileHandle != NULL)
                  {
                      if(index == 0xFF)        //Erase all
                      {
                          assert(PERSISTENCY_FILE_LENGTH_FIELD_LEN == 2);

                          uint16_t fileLen = pFile->pData->header.fileLength[1] +
                                             ((pFile->pData->header.fileLength[0]&0x00ff) << 8) ;

                          size = (uint16_t)(fileLen - offset);
                      }
                      else                    //Erase only as per the Index.
                      {
                          size = serialization_PackedObjectSize(pSerializable);
                          offset = (uint16_t)(offset + (uint16_t)(index*size));
                      }

                      uint8_t *pData = (uint8_t*)osa_Malloc(size);
                      if (pData != NULL)
                      {
                          memset( pData, 0xFF, size);
                          if(fileio_Write(pFileHandle, pData, offset, size) == size)
                          {
                             result = true;
                          }
                          osa_Free(pData);
                      }
                      fileio_Close(pFileHandle);
                  }
              }
          }
    }
    return result;
}

/******************************************************************************
* EXPORTED VARIABLES
******************************************************************************/

/******************************************************************************
* EXPORTED FUNCTIONS
******************************************************************************/

void cwpnode_persistency_WriteAttribute(serialization_ClassType_t attributeType)
{
    cwpnode_persistencyEventData_t newEvt;

    newEvt.attributeType    = attributeType;
    newEvt.operation        = cwpnode_persistencyOperation_Serialize;

    osa_QueueSendToBackCritical(s_PersistencyQueueHandle, &newEvt, 0);
}

void cwpnode_persistency_WriteParamAtIndex(serialization_ClassType_t attributeType, uint8_t index)
{
    cwpnode_persistencyEventData_t newEvt;

    newEvt.attributeType    = attributeType;
    newEvt.index            = index;
    newEvt.operation        = cwpnode_persistencyOperation_SerializeAtIndex;
    osa_QueueSendToBackCritical(s_PersistencyQueueHandle, &newEvt, 0);
}

void cwpnode_persistency_ReadParamAtIndex(serialization_ClassType_t attributeType, uint8_t index)
{
    cwpnode_persistencyEventData_t newEvt;

    newEvt.attributeType    = attributeType;
    newEvt.index            = index;
    newEvt.operation        = cwpnode_persistencyOperation_DeserializeAtIndex;

    osa_QueueSendToBackCritical(s_PersistencyQueueHandle, &newEvt, 0);
}

void cwpnode_persistency_RequestNetworkReset(void)
{
    cwpnode_persistencyEventData_t newEvt;

    newEvt.operation        = cwpnode_persistencyOperation_EraseScenesAndReset;

    osa_QueueSendToBackCritical(s_PersistencyQueueHandle, &newEvt, 0);
}

void cwpnode_persistency_EraseScenesAndReboot(void)
{
    cwpnode_persistencyEventData_t newEvt;

    newEvt.operation        = cwpnode_persistencyOperation_EraseScenesAndReboot;

    osa_QueueSendToBackCritical(s_PersistencyQueueHandle, &newEvt, 0);
}

void cwpnode_persistency_RequestParamsReset(void)
{
    cwpnode_persistencyEventData_t newEvt;
    
    newEvt.operation        = cwpnode_persistencyOperation_ParamsReset;
            
    osa_QueueSendToBackCritical(s_PersistencyQueueHandle, &newEvt, 0);
}

void cwpnode_persistency_DoWork( void )
{
    cwpnode_persistencyEventData_t rxEvt;
    bool_t keepRunning = true;
    
    /* ToCheck: Should we limit cycles instead of infinite loop */
    for (;;)
    {
        if ( keepRunning && osa_QueueReceive(s_PersistencyQueueHandle, &rxEvt, 0))
        {
            switch(rxEvt.operation)
            {
                case cwpnode_persistencyOperation_Serialize:
                    if(serializeAttrib(rxEvt.attributeType) != true)
                    {
                        log_Printfbasic("Failed to store attribute %d", rxEvt.attributeType);
                    }
                    break;
                case cwpnode_persistencyOperation_Deserialize:
                    if(deserializeAttrib(rxEvt.attributeType) != true)
                    {
                        log_Printfbasic("Failed to restore attribute %d", rxEvt.attributeType);
                    }
                    break;
                case cwpnode_persistencyOperation_SerializeIndexToDefault:
                case cwpnode_persistencyOperation_SerializeIndexToErase:
                case cwpnode_persistencyOperation_DeserializeAtIndex:
                    /* Intended fallthrough */
                    /* NOT IMPLEMENTED */
                    break;
                case cwpnode_persistencyOperation_SerializeAtIndex:
                    if(rxEvt.attributeType == serialization_ClassType_ReportingServer_Erase)
                    {
                        removeReportingServerAttribute(rxEvt.attributeType,rxEvt.index);
                    }
                    else
                    {
                        if(serializeIndexAttribute(rxEvt.attributeType,rxEvt.index) != true)
                        {
                            log_Printfbasic("Failed to store attribute %d", rxEvt.attributeType);
                        }
                    }
                    break;    
            #if 0
                case cwpnode_persistencyOperation_ParamsReset:
                    // erase e: and f:
                    if (g_DriveXnvE.pErase)
                    {
                        g_DriveXnvE.pErase();
                    }
                    if (g_DriveXnvF.pErase)
                    {
                        g_DriveXnvF.pErase();
                    }
                    cwpnode_PostPvtEvent( cwpnode_PvtEvent_EraseCompleted, NULL );
                    keepRunning = false;
                    break;
                case cwpnode_persistencyOperation_EraseScenesAndReset:
                    fileio_Remove((uint8_t *)"scenes.conf");
                    lsb_NetworkInstallationSwitch_Commands.resetToFN( lsb_UniverseId_Zigbee, 0 );
                    break;
                case cwpnode_persistencyOperation_EraseScenesAndReboot:
                    fileio_Remove((uint8_t *)"scenes.conf");
                    lsb_Device_Commands.Reboot(lsb_UniverseId_Product, 0);//Reboot the Device.
                    break;
            #endif
                default:
                    break;
            }
        }
        else
        {
            break;
        }
    }
}

// store device conf header and data
static bool_t is_DeviceConf_valid = false;
static persistency_HeaderStructure_t s_DeviceConfHeader;
static filedefs_DeviceConf_Data_t s_DeviceConfData;

static bool_t restoreDeviceConf(void)
{
    // read device conf from internal flash
    filedefs_DeviceConf_Deserialze_Header(&s_DeviceConfHeader);
    filedefs_DeviceConf_Deserialze_Data(&s_DeviceConfData);

    //dump_default_deviceconf();

    if (!is_valid_devconf(&s_DeviceConfHeader))
    {
        log_Printfbasic("not valid device.conf file, so re-write with default settings!");
        return filedefs_DeviceConf_restore_default(&s_DeviceConfHeader, &s_DeviceConfData);
    }

    return true;
}

static void restoreFtaTestData(void)
{
    extern void afMainCommon_DisablePermitJoin(void);
    extern uint8_t emberAfPermitJoin(uint8_t duration, bool broadcastMgmtPermitJoin);
    extern void ENERGYMETERING_TurnOff(void);

	uint8_t fileName[] = "test.data";
	uint8_t buf[4] = {0};
	void* s_pHandler = NULL;

	s_pHandler = fileio_Open(fileName, fileio_Mode_ReadWrite);
	if(s_pHandler != NULL)
	{
		(void)fileio_Read(s_pHandler, &buf[0], 0, 4);
		fileio_Close(s_pHandler);

		if (strncmp((char *)buf, "TEST", 4) == 0)
		{
		    /**
		    * Only testAid and fta will go here. Test Aid and fta need starting stand alone network.
		    * Meanwhile, they should not allow other nodes to join.
		    * Turning off metering is for reducing cpu work load as it is not needed.
		    */
		    ENERGYMETERING_TurnOff();
            afMainCommon_DisablePermitJoin(); // set a flag only
            emberAfPermitJoin(0, false);      // disable permit join
		}
	}
}

static bool_t isResetFromOtaBootloader(void)
{
    uint16_t u16ExtResetInfo = halGetExtendedResetInfo();
    return ((u16ExtResetInfo == 0x201/*GO*/) || (u16ExtResetInfo == 0x203/*BAD*/));
}

static void restoreLightStatusConfData(void)
{
    if (isResetFromOtaBootloader())
    {
        s_sLightStatusConfData.u8OnOff = 0;
    }
    else
    {
        s_sLightStatusConfData.u8OnOff = 1;
    }
}

static void syncMacAddress(void)
{
#ifdef HASEEPROM
    void* s_pHandler = NULL;
    extern uint8_t emLocalEui64[8];
    uint8_t swap_data[8], ori_eui64[8];
    int i;

    // swap to data buffer
    for (i = 0; i < 8; i++)
        swap_data[i] = emLocalEui64[8 - 1 - i];

    s_pHandler = fileio_Open(MEMORYBANK_ROM_FILENAME(54), fileio_Mode_ReadWrite);
    if(s_pHandler != NULL)
    {
        fileio_Read(s_pHandler, ori_eui64, 0, 8);
        bool need_write = false;
        for (i = 0; i < 8; i++)
        {
            if (ori_eui64[i] != swap_data[i])
            {
                need_write = true;
                break;
            }
        }

        if (need_write)
        {
            (void)fileio_Write(s_pHandler, swap_data, 0, 8);
        }

        fileio_Close(s_pHandler);
    }

    log_PrintfbasicMS("memory bank synconized with device config");
#endif
}

#ifdef HASEEPROM
extern cwpnode_MbBuffer_t l_cwpnode_mbBuffer;
#endif

#ifndef DALI_MG12_ENABLED
#define AOC_RANGE_SIZE	4
static bool syncAOCRange(void)
{
#ifdef HASEEPROM
    uint8_t buf[AOC_RANGE_SIZE];

    if (memorybank_Read(MEMORYBANK_117, 0, AOC_RANGE_SIZE, buf) != AOC_RANGE_SIZE)
        return false;

    // device conf's current is little endian
    l_cwpnode_mbBuffer.mb117.minNominalCurrentMSB = s_DeviceConfData.aocMinCurrent[1];
    l_cwpnode_mbBuffer.mb117.minNominalCurrentLSB = s_DeviceConfData.aocMinCurrent[0];
    l_cwpnode_mbBuffer.mb117.maxNominalCurrentMSB = s_DeviceConfData.aocMaxCurrent[1];
    l_cwpnode_mbBuffer.mb117.maxNominalCurrentLSB = s_DeviceConfData.aocMaxCurrent[0];

    // if not same, then overwrite the memory bank value.
    if (memcmp(&l_cwpnode_mbBuffer.mb117, buf, AOC_RANGE_SIZE) != 0)
        return (memorybank_Write(MEMORYBANK_117, 0, AOC_RANGE_SIZE, &l_cwpnode_mbBuffer.mb117) == AOC_RANGE_SIZE);
#endif
    return true;
}
#endif

#define _12NC_BCD_SIZE	6
#define SYNC_MB53_SIZE	(3 + 3 + 2 + _12NC_BCD_SIZE)
static bool sync12NC(void)
{
#ifdef HASEEPROM
    // Initialize the mb53 syncnized data
    uint8_t sync_mb53[SYNC_MB53_SIZE] = {
        APPLICATION_VERSION_MAJOR,
        APPLICATION_VERSION_MINOR,
        APPLICATION_VERSION_REV,
#ifdef DALI_MG12_ENABLED
        0x08,
        11,
#else
        0x0c,
        24,
#endif
        30,     // need read from NFC
        MSB16( BUILDINFO_REVISION ),
        LSB16( BUILDINFO_REVISION ),
        0, 0, 0, 0, 0, 0
    };

    if (memorybank_Read(MEMORYBANK_53, 0, SYNC_MB53_SIZE, &l_cwpnode_mbBuffer.mb53) != SYNC_MB53_SIZE)
        return false;

    // fill mb53 with out band data: subtype & 12NC
    sync_mb53[5] = l_cwpnode_mbBuffer.mb53.familySubtype;

    // convert device conf's 12nc to BCD 12nc
    uint8_t *str_12nc = s_DeviceConfData.tnc;
    uint8_t devconf_bcd_12nc[_12NC_BCD_SIZE];
    for (int i = 0; i < _12NC_BCD_SIZE; i++)
    {
        devconf_bcd_12nc[i] = (str_12nc[2 * i] - '0') << 4 |
                      (str_12nc[2 * i + 1] - '0');
    }
    memcpy(&sync_mb53[8], devconf_bcd_12nc, _12NC_BCD_SIZE);

    // if not same, then overwrite the memory bank value.
    if (memcmp(&l_cwpnode_mbBuffer.mb53, sync_mb53, SYNC_MB53_SIZE) != 0)
    {
        return (memorybank_Write(MEMORYBANK_53, 0, SYNC_MB53_SIZE, sync_mb53) == SYNC_MB53_SIZE);
    }

#endif
    return true;
}

static uint32_t u32BurnTimeStamp = 0;

void cwpnode_BurningTime_SaveWork(void)
{
    uint8_t fileName[] = "burnhours.data";
    void* s_pHandler = NULL;
    uint32_t u32writeCnt = 0;

    if(zigbee_ballastconfigurationserver_atttribute_config.getLampBurningMinutes != NULL)
    {
        uint32_t currentBurnMinutes;
        currentBurnMinutes = zigbee_ballastconfigurationserver_atttribute_config.getLampBurningMinutes();

        s_pHandler = fileio_Open(fileName, fileio_Mode_ReadWrite);
        if(s_pHandler != NULL)
        {
            u32writeCnt = fileio_Write(s_pHandler, (uint8_t *)&(currentBurnMinutes), 0, 4);
            fileio_Close(s_pHandler);
            if (u32writeCnt == sizeof(uint32_t))
            {
                u32BurnTimeStamp = currentBurnMinutes;
            }
            else
            {
                log_Printfbasic("Err::write burn hour data fail:%d", u32writeCnt);
            }
        }
        else
        {
            log_Printfbasic("Err::Cannot open file burnhours.data");
        }
    }
}

void cwpnode_BurningTime_DoWork(void)
{
    if(zigbee_ballastconfigurationserver_atttribute_config.getLampBurningMinutes != NULL)
    {
        uint32_t newBurnMinutes;
        newBurnMinutes = zigbee_ballastconfigurationserver_atttribute_config.getLampBurningMinutes();

        //every 15minutes
        if ((newBurnMinutes - u32BurnTimeStamp) < 15)
        {
            return;
        }

        cwpnode_BurningTime_SaveWork();
    }
}

uint32_t cwpnode_BurningTime_RestoreBurnTime(void)
{
    uint8_t fileName[] = "burnhours.data";
    void* s_pHandler = NULL;
    uint32_t currentBurnMinutes = 0;
    
    if(zigbee_ballastconfigurationserver_atttribute_config.getLampBurningMinutes != NULL)
    {
        s_pHandler = fileio_Open(fileName, fileio_Mode_ReadWrite);
        if(s_pHandler != NULL)
        {
            
            (void)fileio_Read(s_pHandler, (uint8_t *)&currentBurnMinutes, 0, 4);

            //init value in flash is 0xffffffff
            if(currentBurnMinutes == 0xffffffff)
            {
                //init with default 0, and write back to file
                currentBurnMinutes = 0;
                fileio_Write(s_pHandler, (uint8_t *)&currentBurnMinutes, 0, 4);
            }

            fileio_Close(s_pHandler);
        }
    }
    
    u32BurnTimeStamp = currentBurnMinutes;
    return currentBurnMinutes;
}

/******************************************************************************
* EXPORTED FUNCTIONS
******************************************************************************/

void cwpnode_persistency_Init( void )
{
    s_PersistencyQueueHandle = osa_QueueCreate("Persistency_Q", 60, sizeof(cwpnode_persistencyEventData_t));
}

void cwpnode_persistency_TurnOn( void )
{
//    log_Printfbasic("restoreBallastConfigurationAttributes");
    restoreBallastConfigurationAttributes();

//    log_Printfbasic("restoreLevelControlAttributes");
    restoreLevelControlAttributes();

//    log_Printfbasic("restoreLightStatusConfData");
    restoreLightStatusConfData();

//    log_Printfbasic("restoreDdrConfData");
    restoreDdrConfData();
	
//    log_Printfbasic("restoreReportingServer");	
    restoreReportingServer();
	
//    log_Printfbasic("restoreStartupAttributes");
    restoreStartupAttributes();

//    log_Printfbasic("restoreRebroadcastingData");
    restoreRebroadcastingData();

//    log_Printfbasic("restoreDeviceConf");
    restoreDeviceConf();
    is_DeviceConf_valid = true;

    if (!sync12NC())
        log_Printfbasic("failed to sync 12NC from device config!");

#ifndef DALI_MG12_ENABLED
    // sync aoc min, max value to mb117, not needed for easysense node
    if (!syncAOCRange())
        log_Printfbasic("failed to sync AOC range from device config!");
#endif

//    log_Printfbasic("restoreFtaTestData");
    restoreFtaTestData();

    syncMacAddress();
}

const filedefs_DeviceConf_Data_t *cwpnode_persistency_GetDeviceConfData(void)
{
    if (is_DeviceConf_valid)
    {
        return &s_DeviceConfData;
    }
    else
    {
        return NULL;
    }
}

cwpnode_persistencyLightStatusData_t *cwpnode_persistency_GetLightStatusConfData(void)
{
    if (s_sLightStatusConfData.u8OnOff != 0xFF)
    {
        return &s_sLightStatusConfData;
    }
    else
    {
        return NULL;
    }
}

void cwpnode_persistency_StoreLightStatusConfData(void)
{
	uint8_t fileName[] = "lightstatus.conf";
	uint32_t u32writeCnt = 0;
	bool_t bIsOk = false;

    // update level value before store, onoff value is updated by another function.
    s_sLightStatusConfData.u8Level = zigbee_lightcontrolserver_Config.onLevel;

	void* s_pHandler = NULL;
	s_pHandler = fileio_Open(fileName, fileio_Mode_ReadWrite);
	if(s_pHandler != NULL)
	{
        u32writeCnt = fileio_Write(s_pHandler, (uint8_t *)&s_sLightStatusConfData, 0, sizeof(cwpnode_persistencyLightStatusData_t));
        fileio_Close(s_pHandler);
        if (u32writeCnt == sizeof(cwpnode_persistencyLightStatusData_t))
        {
            bIsOk = true;
        }
	}

    log_PrintfCustom("Save: onoff[%d] level[%d]: %s\r",
                        s_sLightStatusConfData.u8OnOff,
                        s_sLightStatusConfData.u8Level,
                        bIsOk ? "success":"fail");
}

bool_t persistency_IsLightOn(void)
{
    assert(s_sLightStatusConfData.u8OnOff != 0xFF);
    return (s_sLightStatusConfData.u8OnOff == 1);
}

void cwpnode_persistency_UpdateLightStatusConfData(bool_t bIsOn)
{
    s_sLightStatusConfData.u8OnOff = bIsOn;
}
/******************************************************************************
* EXPORTED LSB COMMAND INTERFACES
******************************************************************************/
