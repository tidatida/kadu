set (buddies_SRCS
	buddy.cpp
	buddy-additional-data-delete-handler.cpp
	buddy-additional-data-delete-handler-manager.cpp
	buddy-dummy-factory.cpp
	buddy-list.cpp
	buddy-list-mime-data-service.cpp
	buddy-manager.cpp
	buddy-module.cpp
	buddy-preferred-manager.cpp
	buddy-search-criteria.cpp
	buddy-set.cpp
	buddy-shared.cpp
	buddy-storage.cpp
	group.cpp
	group-manager.cpp
	group-shared.cpp
	group-storage.cpp

	model/buddy-data-extractor.cpp
	model/buddy-list-model.cpp
	model/buddy-manager-adapter.cpp
	# will be used when Qt 4.8 .is required
	# model/checkable-buddies-proxy-model.cpp
	model/groups-model.cpp
)

set (buddies_MOC_SRCS
	buddy-additional-data-delete-handler-manager.h
	buddy-shared.h
	buddy-manager.h
	group-manager.h
	buddy-preferred-manager.h
	group-shared.h

	model/buddy-list-model.h
	model/buddy-manager-adapter.h
	# will be used when Qt 4.8 .is required
	# model/checkable-buddies-proxy-model.h
	model/groups-model.h
)

kadu_subdirectory (buddies "${buddies_SRCS}" "${buddies_MOC_SRCS}" "")
