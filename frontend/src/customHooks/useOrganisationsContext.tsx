import React, { createContext, useContext, ReactNode, useState, useEffect } from "react";

import organisationRequests, { Organisation } from "../../API/requests/organisationRequests";
// Define the context type
interface OrganisationsContextType {
    isLoading: boolean,
    selectedOrganisation: Organisation | null
    setSelectedOrganisation: React.Dispatch<React.SetStateAction<Organisation | null>>,

    organisations: Organisation[],
    addOrganisations: (newOrganisations: Organisation[]) => void,
    updateOrganisation: (updatedOrganisation: Organisation) => void,
    deleteOrganisation: (organisationId: string) => void,
};

// Create the context
const OrganisationContext = createContext<OrganisationsContextType | undefined>(undefined);

// Provider component
export const OrganisationsContextProvider: React.FC<{ children: ReactNode }> = ({ children }) => {
    const [isLoading, setIsLoading] = useState(true);
    const [organisations, setOrganisations] = useState<Organisation[]>([]);
    const [selectedOrganisation, setSelectedOrganisation] = useState<null | Organisation>(null);

    const addOrganisations = (newOrganisations: Organisation[]) => {
        setOrganisations((prevOrganisations) => {
            const existingIds = new Set(prevOrganisations.map((org) => org._id));
            const filteredNewOrganisations = newOrganisations.filter((org) => !existingIds.has(org._id));
            return [...prevOrganisations, ...filteredNewOrganisations].sort((a, b) => a.name.localeCompare(b.name));
        });
    }
    const updateOrganisation = (updatedOrganisation: Organisation) => {
        setOrganisations((prevOrganisations) => {
            const index = prevOrganisations.findIndex((org) => org._id === updatedOrganisation._id);
            if (index !== -1) {
                const newOrganisations = [...prevOrganisations];
                newOrganisations[index] = updatedOrganisation;
                return newOrganisations.sort((a, b) => a.name.localeCompare(b.name));
            }
            return prevOrganisations;
        });
    }
    const deleteOrganisation = (organisationId: string) => {
        setOrganisations((prevOrganisations) => {
            const index = prevOrganisations.findIndex((org) => org._id === organisationId);
            if (index !== -1) {
                const newOrganisations = [...prevOrganisations];
                newOrganisations.splice(index, 1);
                return newOrganisations;
            }
            return prevOrganisations;
        }
        )
    }

    console.log("OrganisationsContextProvider - re-rendering: ", organisations, selectedOrganisation);

    useEffect(() => {
        const fetchOrganisations = async () => {
            try {
                setIsLoading(true);
                const response = await organisationRequests.listOrganisation({ pageInfo: { pageIndex: 0, pageSize: 40 }, order: "asc" });
                addOrganisations(Array.isArray(response.organisations) ? response.organisations : []);
            } catch (err) {
                console.error("fetchOrganisations - error: ", err);
            } finally { setIsLoading(false); }
        }
        fetchOrganisations();
    }, []);

    return (
        <OrganisationContext.Provider
            value={{
                isLoading,
                selectedOrganisation, setSelectedOrganisation,
                organisations,
                addOrganisations, updateOrganisation, deleteOrganisation
            }}
        >
            {children}
        </OrganisationContext.Provider>
    );
};

// Custom hook to use the context
export const useOrganisationContext = (): OrganisationsContextType => {
    const context = useContext(OrganisationContext);
    if (!context) {
        throw new Error("useMainContext must be used within a MainProvider");
    }
    return context;
};