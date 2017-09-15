/*
 *	 Author: bwilliams
 *  Created: Oct 22, 2010
 *
 *	Copyright (c) 2010 Vmware, Inc.  All rights reserved.
 *	-- VMware Confidential
 */

#include "stdafx.h"
#include "CPersistenceOutboundChannelAdapterInstance.h"

using namespace Caf;

CPersistenceOutboundChannelAdapterInstance::CPersistenceOutboundChannelAdapterInstance() :
	_isInitialized(false),
	_isRunning(false),
	CAF_CM_INIT_LOG("CPersistenceOutboundChannelAdapterInstance") {
}

CPersistenceOutboundChannelAdapterInstance::~CPersistenceOutboundChannelAdapterInstance() {
}

void CPersistenceOutboundChannelAdapterInstance::initialize(
	const IBean::Cargs& ctorArgs,
	const IBean::Cprops& properties,
	const SmartPtrIDocument& configSection) {
	CAF_CM_FUNCNAME_VALIDATE("initialize");
	CAF_CM_PRECOND_ISNOTINITIALIZED(_isInitialized);
	CAF_CM_VALIDATE_INTERFACE(configSection);

	_id = configSection->findRequiredAttribute("id");
	_configSection = configSection;

	_isInitialized = true;
}

std::string CPersistenceOutboundChannelAdapterInstance::getId() const {
	CAF_CM_FUNCNAME_VALIDATE("getId");
	CAF_CM_PRECOND_ISINITIALIZED(_isInitialized);

	return _id;
}

void CPersistenceOutboundChannelAdapterInstance::wire(
	const SmartPtrIAppContext& appContext,
	const SmartPtrIChannelResolver& channelResolver) {
	CAF_CM_FUNCNAME_VALIDATE("wire");
	CAF_CM_PRECOND_ISINITIALIZED(_isInitialized);
	CAF_CM_VALIDATE_INTERFACE(appContext);
	CAF_CM_VALIDATE_INTERFACE(channelResolver);

	const SmartPtrIMessageChannel errorMessageChannel =
		channelResolver->resolveChannelName("errorChannel");

	const std::string inputChannelStr = _configSection->findRequiredAttribute("channel");
	SmartPtrIMessageChannel inputChannel = channelResolver->resolveChannelName(inputChannelStr);
	SmartPtrIIntegrationObject inputChannelObj;
	inputChannelObj.QueryInterface(inputChannel);

	SmartPtrCPersistenceMessageHandler persistenceMessageHandler;
	persistenceMessageHandler.CreateInstance();
	persistenceMessageHandler->initialize(_configSection);
	SmartPtrIMessageHandler messageHandler;
	messageHandler.QueryInterface(persistenceMessageHandler);

	_messagingTemplate.CreateInstance();
	_messagingTemplate->initialize(
			channelResolver,
			inputChannelObj,
			errorMessageChannel,
			SmartPtrIMessageChannel(),
			messageHandler);
}

void CPersistenceOutboundChannelAdapterInstance::start(
		const uint32 timeoutMs) {
	CAF_CM_FUNCNAME_VALIDATE("start");
	CAF_CM_PRECOND_ISINITIALIZED(_isInitialized);
	CAF_CM_VALIDATE_BOOL(!_isRunning);

	_isRunning = true;
	_messagingTemplate->start(0);
}

void CPersistenceOutboundChannelAdapterInstance::stop(
		const uint32 timeoutMs) {
	CAF_CM_FUNCNAME_VALIDATE("stop");
	CAF_CM_PRECOND_ISINITIALIZED(_isInitialized);
	CAF_CM_VALIDATE_BOOL(_isRunning);

	_isRunning = false;
	_messagingTemplate->stop(0);
}

bool CPersistenceOutboundChannelAdapterInstance::isRunning() const {
	CAF_CM_FUNCNAME_VALIDATE("isRunning");
	CAF_CM_PRECOND_ISINITIALIZED(_isInitialized);

	return _isRunning;
}